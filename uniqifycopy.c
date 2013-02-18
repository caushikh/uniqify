#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_PIPES 1000 
#define BUF_SIZE 100 

static int npipes = 1;

int isAllEmpty(FILE *pipes[MAX_PIPES], int npipes);
int findFirst(char **wlist, char *first, int npipes);

static void phandler(int sig)
{
	int i;
	for (i = 0; i < npipes; i++)
	{
		wait(NULL);
	}
	_exit(EXIT_FAILURE);
}

static void chandler(int sig)
{
	_exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int sortfd[MAX_PIPES][2];
	int suppfd[MAX_PIPES][2];
	char buf[25];
	FILE *insort[MAX_PIPES];
	FILE *outsort[MAX_PIPES];
	FILE *insupp[MAX_PIPES];
	FILE *outsupp[MAX_PIPES];

	char **wlist;
	int isEmpty;
	char *last;
	char *next;
	int pipeno;

	struct sigaction sa;
	struct sigaction sa_chd;

	npipes = atoi(argv[1]);

	if(npipes > MAX_PIPES)
	{
		printf("The limit is %d pipes.\n", MAX_PIPES);
		exit(EXIT_FAILURE);
	}

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = phandler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		perror("sigaction failed\n");
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		perror("sigaction failed\n");
	if (sigaction(SIGHUP, &sa, NULL) == -1)
		perror("sigaction failed\n");

	int i;
	for (i = 0; i < npipes; i++)
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
			sigemptyset(&sa_chd.sa_mask);
			sa_chd.sa_flags = 0;
			sa_chd.sa_handler = chandler;
			if (sigaction(SIGINT, &sa_chd, NULL) == -1)
				perror("sigaction failed\n");
			if (sigaction(SIGQUIT, &sa_chd, NULL) == -1)
				perror("sigaction failed\n");
			if (sigaction(SIGHUP, &sa_chd, NULL) == -1)
				perror("sigaction failed\n");

			insupp[i] = fdopen(suppfd[i][1], "w");
			setlinebuf(insupp[i]);
			printf("I am sorter %d. My processID is %ld.\n",i+1,getpid());
			if (close(sortfd[i][1]) == -1)
				perror("could not close write end\n");
			if (close(suppfd[i][0]) == -1)
				perror("could not close read end\n");
	//		printf("%d : opened communication with suppressor\n",i+1);
			if (sortfd[i][0] != STDIN_FILENO)
			{
				if (dup2(sortfd[i][0], STDIN_FILENO) == -1)
					perror("Could not connect to stdin\n");
				if (close(sortfd[i][0]) == -1)
					perror("could not close pipe\n");
			}
			if (suppfd[i][1] != STDOUT_FILENO)
			{
				if (dup2(suppfd[i][1], STDOUT_FILENO) == -1)
					perror("Could not connect to stdout\n");
				if (close(suppfd[i][1]) == -1)
					perror("could not close pipe\n");
			}

		//	outsort[i] = fdopen(STDIN_FILENO, "r");
		//	insupp[i] = fdopen(STDOUT_FILENO, "w");
		//	insupp[i] = fdopen(sortfd[i][1], "w");
			execl("/bin/sort", "/bin/sort", (char *)NULL);
	#if 0		
			while (!feof(outsort[i]) && !ferror(outsort[i])
				&& fgets(buf, 25, outsort[i]) != NULL)
			{
			//	printf("sorter %d : %s", i+1, buf);
				fputs(buf, insupp[i]);
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
	#endif
		default: /*parent*/
			/* close read end of pipe for master process */
			if (close(sortfd[i][0]) == -1) 
				perror("Could not close pipe\n");
			if (close(suppfd[i][1]) == -1)
				perror("Could not open pipe\n");		
			outsupp[i] = fdopen(suppfd[i][0],"r");
			insort[i] = fdopen(sortfd[i][1], "w");
			break;
		}
	}
	/* write words into pipe */
	pipeno = 0;
	while (scanf("%[A-Za-z]", buf) != EOF)
	{
		scanf("%*c");
		for (i = 0; i < strlen(buf); i++)
			buf[i] = tolower(buf[i]);
		fputs(buf, insort[pipeno]);
		fputc('\n', insort[pipeno]);
		if (pipeno < npipes - 1)
			pipeno++;
		else
			pipeno = 0;
	}
	printf("The words have been dealt.\n");
/*	for (i = 0; i < N_PIPES; i++)
	{
		while (scanf("%[A-z]", buf) != EOF)
		{
			scanf("%*c");
			fputs(buf, insort[i]);
			fputc('\n', in);
		}

		fputs("word\nhari\nis\ngreatest\n", insort[i]);
		fflush(insort[i]);
	}
	*/
	/* close all write pipes */
	for (i = 0; i < npipes; i++)
	{
		if(fclose(insort[i]))
			perror("error closing file stream\n");
	}
	
	/* wait for all children to sort */
/*	for (i = 0; i < npipes; i++)
	{	
		wait(NULL);
		printf("The program gets here.\n");
	}*/
	wlist = malloc(npipes * sizeof(char *));
	for (i = 0; i < npipes; i++)
	{
		wlist[i] = malloc(BUF_SIZE);
	}
	last = malloc(BUF_SIZE);
	next = malloc(BUF_SIZE);
	/* get first word from all pipes */
	if (!isAllEmpty(outsupp, npipes))
	{
		printf("The program gets here.\n");
		for (i = 0; i < npipes; i++)
		{
			if(!feof(outsupp[i]) && !ferror(outsupp[i])
				&& fgets(buf, 25, outsupp[i]) != NULL)
			{
				
	printf("The program gets here.\n");
			//	printf("%s", buf);
				strcpy(wlist[i], buf);
				
			}
			else
			{
				wlist[i] = '\0';
			}
		}
	}
	
	printf("The program gets here.\n");
	/* check if all the pipes are empty */
	
	last[0] = '\0';
	while (!isAllEmpty(outsupp, npipes))
	{
	
		pipeno = findFirst(wlist, next, npipes);
		if (strcmp(next, last) != 0)
		{
			printf("%s", next);
			strcpy(last, next);
		}
		if (!feof(outsupp[pipeno]) && !ferror(outsupp[pipeno])
			&& fgets(buf, 25, outsupp[pipeno]) != NULL)
		{
	//		printf("%s", buf);
			strcpy(wlist[pipeno], buf); 
		}
		else
			wlist[pipeno] = '\0';
	}

	/* do work here */
/*	for (i = 0; i < N_PIPES; i++)
	{
		while (!feof(outsupp[i]) && !ferror(outsupp[i])
			&& fgets(buf, 25, outsupp[i]) != NULL)
		{
			printf("Suppressor Pipe %d : %s", i+1, buf);
		}
	}*/
	for (i = 0; i < npipes; i++)
	{
	//	printf("Suppressor: closing pipe %d\n", i+1);
		if(fclose(outsupp[i]))
			perror("error closing file stream\n");
	/*	if (close(suppfd[i][0]) == -1)
			perror("Could not close pipe\n");*/
	}
	/* wait for all children to sort */
	for (i = 0; i < npipes; i++)
	{	
		wait(NULL);
		printf("The program gets here.\n");
	}

	free(wlist);
	free(next);
	free(last);
	return 0;
}
#if 0
	/* create suppressor process */
	switch(fork()) {
	case -1: 
		perror("fork()\n");
	case 0:
		sleep(3);
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
			while (!feof(outsupp[i]) && !ferror(outsupp[i])
				&& fgets(buf, 25, outsupp[i]) != NULL)
			{
				printf("Suppressor Pipe %d : %s", i+1, buf);
			}
		}
		
		/* get first word from all pipes */
		if (!isAllEmpty(outsupp))
		{
			for (i = 0; i < N_PIPES; i++)
			{
				if(!feof(outsupp[i]) && !ferror(outsupp[i])
					&& fgets(buf, 25, outsupp[i]) != NULL)
				{
					strcpy(wlist[i], buf);
				}
				else 
					wlist[i] = " ";
			}
		}
		
		/* check if all the pipes are empty */
		while (!isAllEmpty(outsupp))
		{
			last = " ";
			pipeno = findFirst(wlist, next);
			if (strcmp(next, last) != 0)
			{
				printf("%s\n", next);
				strcpy(last, next);
			}
			if (!feof(outsupp[pipeno]) && !ferror(outsupp[pipeno])
				&& fgets(buf, 25, outsupp[pipeno]) != NULL)
			{
				strcpy(outsupp[pipeno], buf); 
			}
			else
				outsupp[pipeno] = NULL;
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
		}

		/* wait for all processes to complete work and then close
		 * pipes */
		for (i = 0; i < N_PIPES + 1; i++)
		{
			wait(NULL);
		}

		break;
	}

	return 0;
}
#endif

int isAllEmpty(FILE *pipes[MAX_PIPES], int npipes)
{
	int isEmpty = 0;
	int i;
	for (i = 0; i < npipes; i++)
	{
		if (!feof(pipes[i]) && !ferror(pipes[i]))
		{
			return 0;
	//		isEmpty++;
	//		continue;
		}
	}

//	if (isEmpty == N_PIPES)
		return 1;
//	return 0;
}

int findFirst(char **wlist, char *first, int npipes)
{
	int pipeno;

	/* start with the first word available */
	
	int i;
	for (i = 0; i < npipes; i++)
	{
		if (wlist[i] != '\0')
		{
			pipeno = i;
			strcpy(first, wlist[i]);
			break;
		}
	}
	for (i = pipeno + 1; i < npipes; i++)
	{
		if (wlist[i] == '\0')
			continue;
		if (strcmp(first, wlist[i]) > 0)
		{
			strcpy(first, wlist[i]);
			pipeno = i;
		}
	}
	return pipeno;
}
