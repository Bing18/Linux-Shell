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
void prompt(char *home,int len)
{
	int i,j;
	const char* user = getenv("USER");
	if(user==NULL)
	{
		perror("user name error");
		return ;
	}
	char name[200];
	char path[200];
	getcwd(path,200);
	if(gethostname(name,200)==-1)
	{
		perror("host name error");
		return ;
	}
	if(strcmp(path,home)==0)
		strcpy(path,"~");	
	int flag=0;
	//len is lenght of home path(~)
	if(strlen(path)>=len)
	{
		//printf("%s\n",home);
		j==0;
		for(i=0;i<len;i++)
		{
			if(path[i]!=home[j])
				flag=1;
			j++;
		}
		if(!flag)
		{
			//printf("yes\n");
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
}
