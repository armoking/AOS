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
    4. Создать длительный параллельный процесс (в порожденном процессе выполнять вложенные циклы, каждый на 100 или
    больше итераций, и выводить на экран номер итерации для внешнего цикла). Дождаться нормального завершения
    порожденного процесса. Посмотреть, какой статус будет передан в родительский процесс.
*/

bool childProcessFinished = false;

void action(int sig) {
    childProcessFinished = true;
}

int main() {
    struct sigaction act;   
    act.sa_handler = action;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER; // Не препятствовать получению сигнала при его обработке.
    sigaction(SIGCHLD, &act, NULL); // сигнал посылаемый при изменении статуса дочернего процесса
        

    int pid = fork();
    if (pid == 0) {
        cout << "Child: Pid = " << getpid() << endl;
        for (int i = 0; i < 100; i++) {
            int tmp = i;
            for (int j = 0; j < 1e4; j++) {
                for (int k = 0; k < 1e3; k++) {
                      tmp ^= j + k;
                }
            }
            cout << "Child: loop index = " << i + 1 << endl;
        }
        exit(12);
    } else if (pid > 0) {
        cout << "Parent: Pid = " << getpid() << endl;
        int status;
        int childPid = wait(&status);
        if (childProcessFinished) {
            cout << "SIGCHLD was thrown" << endl;
            if (WIFEXITED(status)) {
                cout << "Parent: child process finished with pid = " << childPid << " and status = " << WEXITSTATUS(status) << endl;
            }
        }
        exit(0);
    }

}
