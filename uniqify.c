#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N_PIPES 2
int main(int argc, char *argv[])
{
	int sortfd[N_PIPES][2];
	int suppfd[N_PIPES][2];
//	int pipefd[2];
//	char buf[25];
//	FILE *in;
	int i;
	for (i = 0; i < N_PIPES; i++)
	{
		/* create pipes for each sorter */
		if (pipe(sortfd[i]) == -1)
			perror("Could not open pipe\n");
		
		/* create child processes */
		switch(fork()) {
		case -1:
			perror("fork\n");
		case 0: /*child*/
			printf("I am sorter %d. My processID is %ld.\n",i+1,getpid());
			if (close(sortfd[i][1]) == -1)
				perror("could not close write end\n");
			if (close(sortfd[i][0]) == -1)
				perror("Could not close pipe\n");
			exit(EXIT_SUCCESS);
			break;
		default: /*parent*/

			/* close read end of pipe for master process */
			if (close(sortfd[i][0]) == -1) 
				perror("Could not close pipe\n");

			if (close(sortfd[i][1]) == -1)		
				perror("could not close pipe parent\n");	

			break;
		}
	}
	printf("I am the parent. My processID is %ld.\n",getpid());		

/*	if (pipe(pipefd) == -1)
		perror("Could not open pipe\n");
	switch(fork()) {
	case -1:
		perror("fork\n");
	case 0: 
		if (close(pipefd[1]) == -1) 
			perror("Could not close pipe\n");
		if (pipefd[0] != STDIN_FILENO)
		{
			if (dup2(pipefd[0], STDIN_FILENO) == -1)
				perror("Could not connect to stdin\n");
			if (close(pipefd[0]) == -1)
				perror("could not close pipe\n");
		}
		sleep(2);
		execl("/bin/sort", "/bin/sort", (char *) NULL);
		if (close(pipefd[0]) == -1)
			perror("error closing pipe\n");
		break;
	default: 		
		if (close(pipefd[0]) == -1)
			perror("Could not close pipe\n");
		if (pipefd[1] != STDOUT_FILENO)
		{
			if (dup2(pipefd[1], STDOUT_FILENO) == -1)
				perror("Could not connect to stdin\n");
			if (close(pipefd[1]) == -1)
				perror("could not close pipe\n");
		}
	
		in = fdopen(STDOUT_FILENO, "w");
		while (scanf("%[A-z]", buf) != EOF)
		{
			scanf("%*c");
			fputs(buf, in);
			fputc('\n', in);
		}
		fclose(in);
		wait(NULL);

		break;
	}
*/
	return 0;
}
