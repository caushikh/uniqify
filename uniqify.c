#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int pipefd[2];
	char buf[25];
	char bufchild[25];
	FILE *in;
	FILE *out;
	FILE *newstdin;
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
		out = fdopen(pipefd[0], "r");
		newstdin = fdopen(0, "w");
		sleep(2);
		while(fgets(bufchild, 25, out) != NULL)
		{
			fprintf(newstdin, "%s", bufchild);
		}
	/*	sleep(2);*/
		fclose(out);
		execl("sort", "sort", (char *) NULL);
		fclose(newstdin);
	/*	if (close(pipefd[0]) == -1)
			perror("error closing pipe\n");*/
		break;
	default: /*parent*/		
		if (close(pipefd[0]) == -1) /* close unused read end */
			perror("Could not close pipe\n");
		in = fdopen(pipefd[1], "w");
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
		fflush(in);
		fclose(in);
		wait(NULL);
		break;
	}

	return 0;
}
