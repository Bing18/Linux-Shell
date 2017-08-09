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
#include <fcntl.h>
int redirection(char *token[],int flag_redirect,char home[],int k)
{
	int i=0;int pid=0;
	int y=0;
	if(k==-1)
		k=0;
	for(i=k;token[i]!=NULL;i++)
	{
		if(strcmp(token[i],"<")==0||strcmp(token[i],">")==0||strcmp(token[i],">>")==0)
			y=1;
	}
	if(y)
	{	
		if(flag_redirect==0)
		pid=fork();
		if(pid==0)
		{
			for(i=k;token[i]!=NULL;i++)
			{
				if(strcmp(token[i],"<")==0)
				{
					int rd_fd=open(token[i+1],O_RDONLY);
					dup2(rd_fd,0);
					close(rd_fd);
				}
				if(strcmp(token[i],">")==0)
				{
					int rd_fd1=creat(token[i+1],0777);
					dup2(rd_fd1,1);
					close(rd_fd1);
				}
				if(strcmp(token[i],">>")==0)
				{
					int rd_fd2=open(token[i+1],O_WRONLY|O_APPEND|O_CREAT,0777);
					dup2(rd_fd2,1);
					close(rd_fd2);
				}
			}
			i=k;int z=0;
			char * tok[10];
			while(token[i]!=NULL)
			{
				if(strcmp(token[i],"<")==0||strcmp(token[i],">")==0||strcmp(token[i],">>")==0)
				{
					i=i+2;
				}
				else
				{
					tok[z++]=token[i];
					i++;
				}
			}
			tok[z]=NULL;
			if(execvp(*tok,tok)<0)
			{
				fprintf(stderr, "Command not found\n");
				return 0;
			}
			//commands(tok,home);
		}
		else if(pid>0 && flag_redirect==0)
			wait(NULL);
		else if(flag_redirect==0 && pid<0)
		{
			perror("error");
			return ;
		}
		return 1;
	}
	else
		return 0;
}
