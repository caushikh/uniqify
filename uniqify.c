#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N_PIPES 3

int main(int argc, char *argv[])
{
	int sortfd[N_PIPES][2];
	int suppfd[N_PIPES][2];
	char buf[25];
	FILE *insort[N_PIPES];
	FILE *outsort[N_PIPES];
	FILE *insupp[N_PIPES];
	FILE *outsupp[N_PIPES];
	int i;
	for (i = 0; i < N_PIPES; i++)
	{
		/* create pipes for each sorter */
		if (pipe(sortfd[i]) == -1)
			perror("Could not open pipe\n");
		/* create pipes for suppressor - sorter communication */
		if (pipe(suppfd[i]) == -1)
			perror("Could not open pipe\n");
		/* create child processes */
		switch(fork()) {
		case -1:
			perror("fork\n");
		case 0: /*child*/
			printf("I am sorter %d. My processID is %ld.\n",i+1,getpid());
			if (close(sortfd[i][1]) == -1)
				perror("could not close write end\n");
			if (close(suppfd[i][0]) == -1)
				perror("could not close read end\n");
	//		printf("%d : opened communication with suppressor\n",i+1);
			outsort[i] = fdopen(sortfd[i][0], "r");
			insupp[i] = fdopen(suppfd[i][1],"w");
		
			while (fgets(buf, 25, outsort[i]) != NULL)
			{
				printf("sorter %d : %s", i+1, buf);
				fputs(buf, insupp[i]);
				printf("gets to this part\n");
			}
			if(fclose(outsort[i]))
				perror("error closing file stream\n");
			if(fclose(insupp[i]))
				perror("error closing file stream\n");
			printf("sorter %d: closed supp comm\n", i+1);

		/*	if (close(sortfd[i][0]) == -1)
				perror("Could not close pipe\n");
			if (close(suppfd[i][1]) == -1)
				perror("could not close read end\n");*/
			exit(EXIT_SUCCESS);
			break;
		default: /*parent*/
			/* close read end of pipe for master process */
			if (close(sortfd[i][0]) == -1) 
				perror("Could not close pipe\n");
			insort[i] = fdopen(sortfd[i][1], "w");
			break;
		}
	}

	/* create suppressor process */
	switch(fork()) {
	case -1: 
		perror("fork()\n");
	case 0:
		printf("I am the suppressor. My processID is %ld.\n", getpid());

		/* attach to suppressor-sorter pipes */
		for (i = 0; i < N_PIPES; i++)
		{
			printf("Suppressor: attaching to pipe %d\n", i+1);
			if (close(suppfd[i][1]) == -1)
				perror("Could not open pipe\n");		
			outsupp[i] = fdopen(suppfd[i][0],"r");
		}

		/* do work here */
		for (i = 0; i < N_PIPES; i++)
		{
			while (fgets(buf, 25, outsupp[i]) != NULL)
			{
				printf("Suppressor Pipe %d : %s", i+1, buf);
			}
		}
		/* close pipes */
		for (i = 0; i < N_PIPES; i++)
		{
		//	printf("Suppressor: closing pipe %d\n", i+1);
			if(fclose(outsupp[i]))
				perror("error closing file stream\n");
		/*	if (close(suppfd[i][0]) == -1)
				perror("Could not close pipe\n");*/
		}
		exit(EXIT_SUCCESS);
		break;
	default:
		for (i = 0; i < N_PIPES; i++)
		{
			fputs("word\n", insort[i]);
			fflush(insort[i]);
			if(fclose(insort[i]))
				perror("error closing file stream\n");
			if(feof(insort[i]))
				printf("end of file is set.\n");
		}
	/*	while (scanf("%[A-z]", buf) != EOF)
		{
			scanf("%*c");
			fputs(buf, in);
			fputc('\n', in);
		}
		fclose(in);
	*/

		/* wait for all processes to complete work and then close
		 * pipes */
		for (i = 0; i < N_PIPES + 1; i++)
		{
			wait(NULL);
		}
	/*	for (i = 0; i < N_PIPES; i++)
		{
			if (close(sortfd[i][1]) == -1)		
				perror("could not close pipe parent\n");	
		}
	*/
		break;
	}

//	printf("I am the parent. My processID is %ld.\n",getpid());		

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
