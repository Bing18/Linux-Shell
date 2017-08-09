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
int piping(char *token[],char home[])
{

	int i=0;	
	int y=0,count=0;
	int index[10];
	//index array for storing indices of next character after pipe
	for(i=0;i<10;i++)
		index[i]=0;
	int ind=0;
	for(i=0;token[i]!=NULL;i++)
	{
		if(strcmp(token[i],"|")==0)
		{
			y=1;
			token[i]=NULL;
			index[ind]=i+1;
			ind++;
			count++;
		}
	}
	ind=0;
	if(y)
	{
		count++;
		int fd_pipe[2*(count-1)];
		for(i=0;i<2*(count-1);i+=2)
		{
			if(pipe(fd_pipe+i)!=0)
			{
				perror("error");
				return;
			}
		}
		int pid=fork();int z;
		if(pid==0)
		{
			dup2(fd_pipe[1],1);
			int l;
			for(l=0;l<2*(count-1);l++)
			{
				if(l!=1)
					close(fd_pipe[l]);
			}
			//check for redirection if yes pass it there else send it to main function(all commands)
			int res=redirection(token,1,home,-1);
			if(res==0)
			{
				if(execvp(*token,token)<0)
				{
					fprintf(stderr, "Command not found\n");
					return 0;
				}
			}
		}
		else
		{	
			z=0;
			wait(NULL);
			while(z<=2*(count-3))
			{
				int pid1=fork();
				if(pid1==0)
				{
					dup2(fd_pipe[z],0);
					dup2(fd_pipe[z+3],1);
					int l=0;
					for(l=0;l<2*(count-1);l++)
					{
						if(l!=z)
							close(fd_pipe[l]);
					}
					//execute
					int res1=redirection(token,1,home,index[ind]);
					if(res1==0)
					{
						if(execvp(token[index[ind]],&token[index[ind]])<0)
						{
							fprintf(stderr, "Command not found\n");
							return 0;
						}
					}
				}
				//else
				ind++;
				z=z+2;
			}
			int p=fork();

			if(p==0)
			{

				dup2(fd_pipe[2*(count-2)],0);
				int l=0;
				for(l=0;l<2*(count-1);l++)
				{
					if(l!=(2*(count-2)))
						close(fd_pipe[l]);
				}
				int res2=redirection(token,1,home,index[ind]);
				if(res2==0)
				{	
					if(execvp(token[index[ind]],&token[index[ind]])<0)
					{
						fprintf(stderr, "Command not found\n");
						return 0;
					}
				}
			}
		}
		for(i=0;i<2*(count-1);i++)
		{
			close(fd_pipe[i]);
		}
		for(i=0;i<=count;i++)
		{
			wait(NULL);
		}
		return 1;
	}
	else
		return 0;
}
