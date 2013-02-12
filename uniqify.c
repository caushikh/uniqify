#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	int pipefd[2];
	char buf[25];
	FILE *in;
//	FILE *out;
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
	//	out = fdopen(pipefd[1], "r");
		sleep(2);
		execl("sort", "sort", (char *) NULL);
		if (close(pipefd[0]) == -1)
			perror("error closing pipe\n");
		break;
	default: /*parent*/		
		if (close(pipefd[0]) == -1) /* close unused read end */
			perror("Could not close pipe\n");
		in = fdopen(pipefd[0], "w");
		while (scanf("%['a'-'z']", buf) != 0)
		{
			scanf("%['a'-'Z']", buf);
			fputs(buf, in);
		}
		wait();
		if (close(pipefd[1]) == -1)
			perror("error closing pipe\n");
		break;
	}

	fclose(in);
	return 0;
}
