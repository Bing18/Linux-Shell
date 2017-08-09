#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>
#include "header.h"
#include "header1.h"
#include "header2.h"
int errno;
sigjmp_buf env;
int main()
{

	const char* user = getenv("USER");
	//user name	
	if(user==NULL)
	{
		perror("user name error");
		return -1;	
	}
	char name[200];
	char home[200];int i=0;
	int j;
	//home contains absolute path of "~"
	getcwd(home,200);
	int len=strlen(home);
	const char s[]=" \t\n";
	const char s1[]=";";
	while(1)
	{
		char path[200];
		getcwd(path,200);
		if(gethostname(name,200)==-1)
		{
			perror("host name error");
			return -1;
		}
		if(strcmp(path,home)==0)
			strcpy(path,"~");	
		int flag=0;
		//len is lenght of home path(~)
		if(strlen(path)>=len)
		{
			j==0;
			for(i=0;i<len;i++)
			{
				if(path[i]!=home[j])
					flag=1;
				j++;
			}
			if(!flag)
			{
				char temp1[100];
				strcpy(temp1,"~");
				j=1;
				for(i=strlen(home);i<strlen(path);i++)
					temp1[j++]=path[i];
				temp1[j]='\0';
				strcpy(path,temp1);
			}
		}
		printf("%s@%s::%s$ ",user,name,path);
		//reading input
		char str[200];
		fgets(str,sizeof(str),stdin);
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
			char q[100];
			int bg=0;
			strcpy(q,primary[k]);
			char * token[10];i=0;
			token[0]=strtok(q,s);
			while(token[i]!=NULL)
			{
				i++;
				token[i]=strtok(NULL,s);
			}
			//command &
			if(i>1 && token[i-1][0]=='&')
			{
				token[i-1]=NULL;
				bg=1;
				if(strcmp(token[0],"echo")==0)
					echo(token,bg);
				else
				exec_command(token,bg);
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
				if(strcmp(token[0],"echo")==0)
					echo(token,bg);
				else
				exec_command(token,bg);
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
				if(token[1]!=NULL){
				strcpy(x,token[1]);
				if(x[0]=='~')
				{
					char temp[200];
					strcpy(temp,home);
					int i=strlen(home);
					for(j=1;j<strlen(token[1]);j++)
					{
						temp[i]=x[j];
						i++;
					}
					temp[i]='\0';
					if(chdir(temp)<0)
					{
						perror("Error");
					}
				}
				else
				{
					if(chdir(token[1])<0)
						perror("Error");
				}
			}
			} 
			else if(strcmp(token[0],"quit")==0)
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
				char temp1[1256],temp2[1256],name[1256];
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
					/*scan the stat file*/
					fscanf(pointer,"%d %*s %c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %lu %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d",&pid,&state,&size);  
					fclose(pointer);
					printf("PID -- %d\nProcess Status -- %c\nMemory -- %lu\n",pid,state,size);
					readlink(temp2,name,1256);
					if(strstr(name,home))
					{
						char tilda[1256]="~";
						strcat(tilda,name+strlen(home));
						printf("Executable Path -- %s\n",tilda); 
					}
					else
						printf("Executable Path -- %s\n",name);
				}
				else
					perror("No such process");
			}
			else
				exec_command(token,bg);
			k++;
		}
	}
	return 0;
}
