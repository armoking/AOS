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
    11. Написать программу, позволяющую использовать sigaction для реализации примера синхронизации процессов. Выполнить
    эту программу и объяснить ее поведение. Использовать sigsuspend и sigprocmask.
*/

void firstAction(int sig) {
    cout << "firstAction " << sig << endl;
}

void secondAction(int sig) {
    cout << "secondAction " << sig << endl;
}

int main() {
    static struct sigaction action, childAction;

    sigset_t mask, oldMask;
    

    action.sa_handler = firstAction;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGUSR1, &action, NULL);

    childAction.sa_handler = secondAction;
    sigemptyset(&childAction.sa_mask);
    childAction.sa_flags = 0;
    sigaction(SIGUSR2, &childAction, NULL);

    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, &oldMask);
/*
    Системный вызов sigprocmask используется для того, чтобы изменить список блокированных в данный момент сигналов. Работа
    этой функции зависит от значения параметра how следующим образом:

    SIG_BLOCK       -   Набор блокируемых сигналов - объединение текущего набора и аргумента set.
    
    SIG_UNBLOCK     -   Сигналы, устанавливаемое значение битов которых равно set, удаляются из списка блокируемых сигналов. Допускается
    разблокировать незаблокированные сигналы.
    
    SIG_SETMASK     -   Набор блокируемых сигналов приравнивается к аргументу set.
    Если значение поля oldset не равно нулю, то предыдущее значение маски сигналов записывается в oldset.

*/

    int pid = fork();
    if (pid == 0) {
        cout << "Child: Pid = " << getpid() << endl;
        int parentId = getppid();
        for (int i = 0; i < 5; i++) {
            kill(parentId, SIGUSR1);
            sigsuspend(&oldMask); // Системный вызов sigsuspend временно изменяет значение маски блокировки сигналов
            // процесса на указанное в mask, и затем приостанавливает работу процесса до получения соответствующего
            // сигнала.  
        }

        exit(12);
    } else if (pid > 0) {
        cout << "Parent: Pid = " << getpid() << endl;   
        for (int i = 0; i < 5; i++) {
            sigsuspend(&oldMask);    
            kill(pid, SIGUSR2);
        }

        int status;
        int childPid = wait(&status);
        if (WIFEXITED(status)) {
            cout << "Parent: child process finished with pid = " << childPid << " and exit code = " << WEXITSTATUS(status) << endl;
        } else {
            cout << "Parent: child process finished abnormally " << childPid << " status = " << status << endl;
        }
    }
}
