#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	int pipefd[2];
	char buf[25];
	char *bufchild;
	FILE *in;
	FILE *out;
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
		sleep(2);
		bufchild = fgets(bufchild, 25, out);
		printf("%s\n", bufchild);
		
	/*	sleep(2);
		execl("sort", "sort", (char *) NULL);
	*	if (close(pipefd[0]) == -1)
			perror("error closing pipe\n");*/
		fclose(out);
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
		scanf("%[A-z]", buf);
	/*	printf("%s\n", buf);*/
		fputs(buf, in);
		fputc('\n', in);
		fflush(in);
		wait();
		fclose(in);
	/*	if (close(pipefd[1]) == -1)
			perror("error closing pipe\n");
	*/	break;
	}

	return 0;
}
