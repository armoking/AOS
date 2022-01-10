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
    8. Включить в порожденный процесс системный вызов signal, переопределяющий стандартную реакцию на сигнал (для
    внешнего цикла установить в функции максимальное значение параметра цикла). Что изменится?
*/

bool childProcessFinished = false;

int loopIndex = 0;
void action(int sig) {
    cout << "SIGUSR1 comes in child process" << endl;
    cout << "Set up index to 100" << endl;
    loopIndex = 100;
}

bool doAction = true;
bool doAnything = true;

int main() {
    int pid = fork();
    if (pid == 0) {
        cout << "Child: Pid = " << getpid() << endl;
        if (!doAnything) {
            // pass
        } else if (doAction) {
            signal(SIGUSR1, action);  
        } else {
            signal(SIGUSR1, SIG_DFL);    
        }
        for (loopIndex = 1; loopIndex <= 100; loopIndex++) {
            int tmp = 0;
            for (int j = 0; j < 1e7; j++) {
                tmp ^= j;
            }
            cout << "Child process loop index = " << loopIndex << endl;
        }
        exit(12);
    } else if (pid > 0) {
        cout << "Parent: Pid = " << getpid() << endl;
        sleep(1);
        kill(pid /* child pid */, SIGUSR1);
        int status;
        int childPid = wait(&status);
        if (WIFEXITED(status)) {
            cout << "Parent: child process finished with pid = " << childPid << " and exit code = " << WEXITSTATUS(status) << endl;
            cout << "status = " << status << endl;
        } else {
            cout << "Parent: child process finished abnormally " << childPid << " status = " << status << endl;
            cout << "SIGUSR1 = " << SIGUSR1 << endl;
        }
    }

}
