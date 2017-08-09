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
void exec_command(char *token [],int bg)
{
	pid_t pid;
	pid=fork();
	int status;
	//background process
	if(bg)
	{
		//child process
		if(pid==0)
		{
			//independent process
			setpgid(0,0);
			int val=execvp(*token,token);
			if(val==-1)
			{
				perror("error");
				exit(1);
			}
			// else
				// exit(0);
		}
		else if(pid<0)
		{
			perror("error");
			return;
		}
		else
		{
			printf("%d\n",pid);
			// wait(NULL);
		}

	}
	//not a bacground process
	else 
	{

		//child process
		if(pid==0)
		{
			int val=execvp(*token,token);
			if(val<0)
			{
				perror("error");
			}
		}
		//parent process
		else if(pid>0)
		{
			waitpid(pid,&status,0);
		}
	}
}
