#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char ** argv) {
	const char * usage =
		"jerk runs two commands and connects their stdin and stdout in a cycle\n"
		"Usage:\n"
		"    jerk <command1> [arg1 ...] ; <command2> [arg1 ...]\n";

	if (argc < 3) {
		fprintf(stderr, usage);
		return 1;
	}

	char ** nargv;
	char ** c1argv = NULL;
	char ** c2argv = NULL;
	int err;
	nargv = malloc(sizeof(char **) * argc);
	memcpy(nargv, argv, sizeof(char **) * argc + 1);
	int i;
	for (i = 0; i < argc; ++i) {
		if (strcmp(nargv[i], ";") == 0) {
			nargv[i] = NULL;
			if (c2argv == NULL && i + 1 != argc)
				c2argv = &nargv[i+1];
		}
	}
	c1argv = &nargv[1];
	nargv[argc] = NULL;
	if (c2argv == NULL) {
		fprintf(stderr, "Separate commands with a ';'\nMake sure to escape it from your shell.\nSee usage for more information\n\n");
		return 1;
	}

	pid_t pid1, pid2;
	int c1toc2[2];
	int c2toc1[2];

	err = pipe(c1toc2);
	if (err == -1) {
		perror("pipe");
		return 1;
	}
	err = pipe(c2toc1);
	if (err) {
		perror("pipe");
		return 1;
	}

	pid1 = fork();
	if (pid1 < -1) {
		perror("fork");
		return 1;
	}
	if (pid1 == 0) {
		/* child1 */
		err = dup2(c2toc1[0], 0);
		if (err == -1) {
			perror("dup2");
			return 1;
		}
		err = dup2(c1toc2[1], 1);
		if (err == -1) {
			perror("dup2");
			return 1;
		}
		err = close(c1toc2[0]);
		if (err) {
			perror("close");
			return 1;
		}
		err = close(c1toc2[1]);
		if (err) {
			perror("close");
			return 1;
		}
		err = close(c2toc1[0]);
		if (err) {
			perror("close");
			return 1;
		}
		err = close(c2toc1[1]);
		if (err) {
			perror("close");
			return 1;
		}
		err = execvp(c1argv[0], c1argv);
		if (err) {
			perror("close");
			return 1;
		}
	} else {
		/* parent */
		pid2 = fork();
		if (pid2 < -1) {
			perror("fork");
			return 1;
		}
		if (pid2 == 0) {
			/* child2 */
			err = dup2(c1toc2[0], 0);
			if (err == -1) {
				perror("dup2");
				return 1;
			}
			err = dup2(c2toc1[1], 1);
			if (err == -1) {
				perror("dup2");
				return 1;
			}
			err = close(c1toc2[0]);
			if (err) {
				perror("close");
				return 1;
			}
			err = close(c1toc2[1]);
			if (err) {
				perror("close");
				return 1;
			}
			err = close(c2toc1[0]);
			if (err) {
				perror("close");
				return 1;
			}
			err = close(c2toc1[1]);
			if (err) {
				perror("close");
				return 1;
			}
			err = execvp(c2argv[0], c2argv);
		} else {
			/* parent */
			close(c1toc2[0]);
			if (err) {
				perror("close");
				return 1;
			}
			close(c1toc2[1]);
			if (err) {
				perror("close");
				return 1;
			}
			close(c2toc1[0]);
			if (err) {
				perror("close");
				return 1;
			}
			close(c2toc1[1]);
			if (err) {
				perror("close");
				return 1;
			}
			free(nargv);
			wait(0);
			wait(0);
		}
	}

	return 0;
}
