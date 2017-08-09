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
void exit_child(void)
{
	pid_t child;
	int status;
	while ((child = waitpid(-1, &status, WNOHANG)) > 0)
	{
		printf("\nChild %d terminated\n",child);
	}
	siglongjmp(env,1);
}
