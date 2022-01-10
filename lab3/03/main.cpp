#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

using std::cin;
using std::cout;
using std::endl;
using std::cerr;

/*
    3. Написать программу, исключающую появление зомби для завершающихся порожденных процессов.
*/

void action(int sig) {
   // https://www.opennet.ru/man.shtml?topic=waitpid&category=2&russian=0
    return;
    int processId, status;
    while ((processId = waitpid(-1, &status, 0)) != -1) {
        if (processId == -1) {
            cerr << "Error" << endl;
        } else if (WIFEXITED(status)) {
            int exitStatus = WEXITSTATUS(status);
            cout << "Child process finished with pid " << processId << " and exitCode " << exitStatus << endl;
        }
    }
}

int main() {
    struct sigaction act;   
    act.sa_handler = action;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NOCLDWAIT; // Не препятствовать получению сигнала при его обработке.
    sigaction(SIGCHLD, &act, NULL); // сигнал посылаемый при изменении статуса дочернего процесса
    
    for (int index = 0; index < 8; index++) {
        int pid = fork();
        if (pid == 0) {
            // child process
            cout << "Child " << index + 1 << ": " << getpid() << endl;
            sleep(2);
            exit(1);
        }
    }

    pause();
}
