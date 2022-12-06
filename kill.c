#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int a = 0;

static void signal_handler(int signo) {
	if (signo == SIGUSR1){
		printf("%d. Захвачен сигнал SIGUSR1!\n", a);
		a++;
	}
}

int main(void) {
	struct sigaction sigAction;
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGUSR1);
	sigAction.sa_handler=signal_handler;
	sigAction.sa_mask=mask;
	int flag = 0;
	pid_t pid;	
	pid = fork();
	while(1){
		if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
			fprintf(stderr, "Невозможно обработать SIGUSR1!\n");
			exit(EXIT_FAILURE);
		}	
		if (pid == 0) {
			kill(getppid(), SIGUSR1);
			sleep(1);
			printf("Процесс - потомок:\n");
		} 
		if (pid > 0) {
			sleep(1);
			printf("Родительский процесс:\n");
			kill(pid, SIGUSR1);
		} 
	sleep(1);
	}
	return 0;
}

