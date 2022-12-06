#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int a = 1;

void handler(int signo, siginfo_t *si, void *context) { 
  if (signo == SIGUSR1){
    printf("%d. Захвачен сигнал SIGUSR1(%d).\n",a++, si->si_value.sival_int);	//дочерний процесс обрабатывает сигнал, выводит полученный номер, текстовое представление сигнала, доп.информацию
  }
  //exit(EXIT_SUCCESS);
}

int main(void) {
  pid_t pid, cpid;	
  int flag;
  struct sigaction sigAction;	//структура для управления сигналами
  union sigval value;		//переменная для передачи доп.информации

  sigAction.sa_sigaction = handler;
  sigAction.sa_flags = SA_SIGINFO;		//теперь поле sa_sigaction определяет используемую функцию обработки сигналов с другим прототипом void my_handler(int signo, siginfo_t *si, void *ucontext)
						//1-номер сигнала, 2-структура siginfo_t, 3- структура ucontext_t

  if (sigaction(SIGUSR1, &sigAction, 0) != 0) {			
    fprintf(stderr, "Невозможно обработать SIGINT!\n");
    exit(EXIT_FAILURE);
  }

  if (signal(SIGTERM, SIG_DFL) == SIG_ERR) {		//восстановить поведение по умолчанию для сигнала(SIG_DFL для выполнения стандартных действий)
    fprintf(stderr, "Невозможно обработать SIGINT!\n");
    exit(EXIT_FAILURE);
  }

  pid = fork(); //создаем дочерний процесс
  
  if (pid == 0) {	
      int i;
      for (i = 0; i < 10;i++){			        
	pause();  		//приостанавливает вызывающий процесс до тех пор, пока не будет перехвачен какой-то сигнал
      }
     // sleep(1);
      //pause();
  } 
  
  if (pid > 0) {
    int i;   
    for (i = 0; i < 10; i++){
      value.sival_int = i;
      sigqueue(pid, SIGUSR1, value);		//родительский процесс посылает дочернему процессу сигнал с дополнительной информацией
	sleep(1);
    }
    kill(pid, SIGTERM);		//посылаем сигнал дочернему процессу, который завершает его работу
    cpid = wait(&flag);		//ждем завершения дочернего процесса и завершаем основной 
    printf("Процесс потомок завершился.\n");
  }
	
	

  return 0;
}
