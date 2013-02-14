#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int pipefd[2];
	char buf[25];
//	char bufchild[25];
	FILE *in;
//	FILE *out;
//	FILE *newstdin;
//	scanf("%s", buf);
//	printf("%s\n", buf);

	if (pipe(pipefd) == -1)
		perror("Could not open pipe\n");
	switch(fork()) {
	case -1:
		perror("fork\n");
	case 0: /*Child*/
		if (close(pipefd[1]) == -1) /* close unused write end */
			perror("Could not close pipe\n");
		if (pipefd[0] != STDIN_FILENO)
		{
			if (dup2(pipefd[0], STDIN_FILENO) == -1)
				perror("Could not connect to stdin\n");
			if (close(pipefd[0]) == -1)
				perror("could not close pipe\n");
		}
	//	out = fdopen(pipefd[0], "r");
	//	newstdin = fdopen(STDIN_FILENO, "w");
		sleep(2);
	/*	whil9e(fgets(bufchild, 25, out) != NULL)
		{
			if (fputs(bufchild,newstdin) == EOF)
				printf("error on fputs\n");
			if (fputs(bufchild, newstdin))
				printf("fputs works.\n");
			else
				printf("there's a mistake.\n");
		}
		sleep(2);
		fclose(out);
		fclose(newstdin);
		scanf("%s", buf);
		printf("The word is %s",buf);*/
//		write(STDIN_FILENO,"hari\ngokul",10);
		execl("/bin/sort", "/bin/sort", (char *) NULL);
		if (close(pipefd[0]) == -1)
			perror("error closing pipe\n");
		break;
	default: /*parent*/		
		if (close(pipefd[0]) == -1) /* close unused read end */
			perror("Could not close pipe\n");
		if (pipefd[1] != STDOUT_FILENO)
		{
			if (dup2(pipefd[1], STDOUT_FILENO) == -1)
				perror("Could not connect to stdin\n");
			if (close(pipefd[1]) == -1)
				perror("could not close pipe\n");
		}
	
		in = fdopen(STDOUT_FILENO, "w");
	/*	while (scanf("%['a'-'Z']", buf) != 0)
		{
			fputs(buf, in);
		}
		fclose(in);
		wait();
		if (close(pipefd[1]) == -1)
			perror("error closing pipe\n");
	*/
		while (scanf("%[A-z]", buf) != EOF)
		{
	/*	printf("%s\n", buf);*/
			scanf("%*c");
			fputs(buf, in);
			fputc('\n', in);
	/*	if (close(pipefd[1]) == -1)
			perror("error closing pipe\n");
	*/	}
		fclose(in);
		wait(NULL);

		break;
	}

	return 0;
}
