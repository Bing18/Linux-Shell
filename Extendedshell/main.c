#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>
#include "echo.h"
#include "piping.h"
//#include "header2.h"
#include "exec_command.h"
#include "prompt.h"
#include "redirection.h"
sigjmp_buf env;
char home[200];
int len;
struct node{
	int pid;
	char pid_name[100];
	struct node* next;
};
struct node * list=NULL;
void insert(int pid,char pid_name[])
{
	struct node * temp=(struct node*)malloc(sizeof(struct node));
	temp->pid=pid;
	strcpy(temp->pid_name,pid_name);
	// temp->pid_name=pid_name;
	temp->next=NULL;
	if(list==NULL)
		list=temp;
	else
	{
		struct node * var=list;
		while(var->next!=NULL)
		{
			var=var->next;
		}
		var->next=temp;
	}
}
void deletelist(int pid)
{
	struct node * temp;
	struct node * temp1;
	temp=list;temp1=list;
	int pos=0,index;
	if(temp->pid==pid)
	{
		list=temp->next;
		return;
	}
	while(temp->next!=NULL)
	{
		if(temp->next->pid==pid )
		{
			temp->next=temp->next->next;
			break;
		}
		if(temp!=NULL)
			temp=temp->next;
		else
			break;
	}
}
void exit_child(void)
{
	pid_t child;
	int status;
	while ((child = waitpid(-1, &status, WNOHANG)) > 0)
	{
		deletelist(child);
		printf("\nChild %d terminated\n",child);
	}
	siglongjmp(env,1);
}
void printlist(void)
{
	struct node* temp=list;
	int i=1;
	while(temp!=NULL)
	{
		printf("[%d] %s [%d]\n",i,temp->pid_name,temp->pid);
		temp=temp->next;
		i++;
	}

}
int pidtemp=-1;
void funct(sig)
{
	
	// char c;
	signal(sig,SIG_IGN);
	if(pidtemp!=-1)
		kill(pidtemp,SIGKILL);
	else
	{
	printf("\n");
	prompt(home,len);
	fflush(stdout);
	}
}
int errno;
int main()
{

	int i=0;
	//for quit command
	int qu=0;
	int j;
	//home contains absolute path of "~"
	getcwd(home,200);
	len=strlen(home);
	const char s[]=" \t\n";
	const char s1[]=";";
	while(1)
	{
		pidtemp=-1;
		if(!qu)
		prompt(home,len);
		else
			break;
		//reading input
		char str[200];
		signal(SIGINT,funct);

		char *ctrld=fgets(str,sizeof(str),stdin);
		if(ctrld==NULL)
		{
			printf("\n");
			exit(0);
		}
		str[strlen(str)-1]='\0';
		//seperating them with delimitter ;
		char *primary[10];int k=0;
		primary[0]=strtok(str,s1);
		while(primary[k]!=NULL)
		{
			k++;
			primary[k]=strtok(NULL,s1);
		}
		int count=k;
		k=0;
		while(k<count)
		{
			char q[100],q1[100],q2[100];
			strcpy(q,primary[k]);
			strcpy(q1,primary[k]);
			strcpy(q2,primary[k]);
			char * to[10];
			i=0;
			to[0]=strtok(q2,s);
			while(to[i]!=NULL)
			{
				i++;
				to[i]=strtok(NULL,s);
			}
			int ispipe=piping(to,home);
			int isredirect=0;
			if(!ispipe)
			{
				char * tok[10];
				i=0;
				tok[0]=strtok(q,s);
				while(tok[i]!=NULL)
				{
					i++;
					tok[i]=strtok(NULL,s);
				}
				isredirect=redirection(tok,0,home);
				if(!isredirect)
				{
					//call commands
					//printf("non redirect\n");
					char * token[10];
					i=0;
					token[0]=strtok(primary[k],s);
					while(token[i]!=NULL)
					{
						i++;
						token[i]=strtok(NULL,s);
					}
					//printf("%d\n",i);
					int bg=0;
					//command &
					//if commands are more than one
					if(i>1 && token[i-1][0]=='&')
					{
						token[i-1]=NULL;
						bg=1;
						int val;
						if(strcmp(token[0],"echo")==0)
							val=echo(token,bg);
						else
							val=exec_command(token,bg);
						pidtemp=val;
						if(val!=0)
							insert(val,token[0]);
						struct sigaction sa;
						sigfillset(&sa.sa_mask);
						sa.sa_handler=(void *)exit_child;
						sa.sa_flags=0;
						sigaction(SIGCHLD,&sa,NULL);
						sigsetjmp(env,1);
						bg=0;
					}
					//command&
					else if(token[0][strlen(token[0])-1]=='&')
					{
						char sample[100];
						strncpy(sample,token[0],strlen(token[0])-1);
						strcpy(token[0],sample);
						token[0][strlen(token[0])-1]='\0';
						bg=1;
						int val;
						if(strcmp(token[0],"echo")==0)
							val=echo(token,bg);
						else
							val=exec_command(token,bg);
						pidtemp=val;
						if(val!=0)
							insert(val,token[0]);
						struct sigaction sa;
						sigfillset(&sa.sa_mask);
						sa.sa_handler=(void *)exit_child;
						sa.sa_flags=0;
						sigaction(SIGCHLD,&sa,NULL);
						sigsetjmp(env,1);
						bg=0;
					}
					else if(strcmp(token[0],"cd")==0)
					{
						char x[200];
						strcpy(x,token[1]);
						if(x[0]=='~')
						{
							char temp[200];
							strcpy(temp,home);
							int i=strlen(home);
							int j;
							for(j=1;j<strlen(token[1]);j++)
							{
								temp[i]=x[j];
								i++;
							}
							temp[i]='\0';
							chdir(temp);
						}
						else
							chdir(token[1]);
					} 
					else if(strcmp(token[0],"exit")==0)
					{
						return 0;
					}
					else if(strcmp(token[0],"pwd")==0)
					{
						char path[200];
						getcwd(path,200);
						printf("%s\n",path);
					}
					else if(strcmp(token[0],"echo")==0)
					{
						echo(token,bg);
					}
					else if(strcmp(token[0],"pinfo")==0)
					{
						FILE *pointer;
						char temp1[1000],temp2[1000],name[1000];
						char state;
						long unsigned int size;
						int pid;
						if(token[1]==NULL)
						{
							//process id
							sprintf(temp1,"/proc/%d/stat",getpid());
							//link to the executable of this process
							sprintf(temp2,"/proc/%d/exe",getpid());
						}
						else
						{
							//filename of exectable(token[1])
							sprintf(temp1,"/proc/%s/stat",(token[1]));
							sprintf(temp2,"/proc/%s/exe",(token[1]));
						}
						if((pointer=fopen(temp1,"r"))!=NULL)
						{
							//scan the stat file
							fscanf(pointer,"%d %*s %c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %lu %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d",&pid,&state,&size);  
							fclose(pointer);
							printf("PID -- %d\nProcess Status -- %c\nMemory -- %lu\n",pid,state,size);
							readlink(temp2,name,256);
							if(strstr(name,home))
							{
								char tilda[1000]="~";
								strcat(tilda,name+strlen(home));
								printf("Executable Path -- %s\n",tilda);    //print print the stuff
							}
							else
								printf("Executable Path -- %s\n",name);
						}
						else
							perror("No such process");
					}
					else if(strcmp(token[0],"jobs")==0)
					{
						printlist();
					}
					else if(strcmp(token[0],"killallbg")==0)
					{
						struct node* temp=list;
						while(temp!=NULL)
						{
							kill(temp->pid,SIGKILL);
							// deletelist(temp->pid);
							temp=temp->next;
						}
					}
					else if(strcmp(token[0],"fg")==0)
					{
						if(token[1]!=NULL && token[2]==NULL)
						{
							struct node* temp=list;
							int val,status,count=1;
							if(temp==NULL)
							{
								fprintf(stderr,"Invalid job number\n");
							}
							else
							{
								while(temp!=NULL)
								{
									if(count==atoi(token[1]))
									{
										val=temp->pid;
										break;
									}
									count++;	
									temp=temp->next;
								}
								pidtemp=val;
								deletelist(val);
								waitpid(val,&status,0);
							}
						}
						else
							fprintf(stderr,"only one job has to be given\n");
					}
					else if(strcmp(token[0],"kjob")==0)
					{
						if(token[1]!=NULL && token[2]!=NULL && token[3]==NULL)
						{
							struct node *temp=list;
							int count=1,val;
							if(temp==NULL)
							{
								fprintf(stderr,"Invalid job number\n");
							}
							else
							{
								while(temp!=NULL)
								{
									if(count==atoi(token[1]))
									{
										val=temp->pid;
										break;
									}
									count++;	
									temp=temp->next;
								}
								// deletelist(val);
							}
							int sig=atoi(token[2]);
							killpg(val,sig);
						}
						else
							fprintf(stderr,"Invalid syntax\n");
					}
					else if(strcmp(token[0],"quit")==0)
					{
						qu=1;
					}
					else
					{
						int val;
						val=exec_command(token,bg);	
						pidtemp=val;
					}
				}
			}
			k++;
		}
	}
	return 0;
}

