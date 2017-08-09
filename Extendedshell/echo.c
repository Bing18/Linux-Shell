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
sigjmp_buf env;
int echo(char *token[],int bg)
{
	pid_t pid;
	pid=fork();
	if(pid==0)
	{
		if(bg)
		setpgid(0,0);
		int i,j,k=0;
		for(i=1;token[i]!=NULL;i++)
		{
			//removing quotes
			char quote[100];
			k=0;
			for(j=0;token[i][j]!='\0';j++)
			{
				if(token[i][j]!='\"')
				{
					quote[k++]=token[i][j];
				}
			}
			quote[k]='\0';
			strcpy(token[i],quote);
			//checking for $
			char s[100],a[100];
			strcpy(s,quote);
			if(token[i][0]=='$')
			{
				int z=1;j=0;
				char a[100];
				for(z=1;s[z]!='\0';z++)
					a[j++]=s[z];
				a[j]='\0';
				strcpy(token[i],getenv(a));
			}
			printf("%s ",token[i]);
		}
		printf("\n");
	}
	else if(pid==-1)
	{
		perror("error");
		return ;
	}
	else
	{
		if(bg)
		{
			printf("%d\n",pid);
			return pid;
		}
		else
		{
			wait(NULL);
			return 0;
		}
	}
}
