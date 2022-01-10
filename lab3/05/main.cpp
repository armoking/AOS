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
    5. Из родительского процесса послать в порожденный процесс сигнал (SIGUSR1). Посмотреть, какой статус будет передан
    в родительский процесс в этом случае. Сколько итераций выполнится в порожденном процессе?
*/

bool childProcessFinished = false;

void action(int sig) {
    cout << "SIGUSR1 comes in child process" << endl;
    exit(15);
}


int main() {
    int pid = fork();
    if (pid == 0) {
        cout << "Child: Pid = " << getpid() << endl;
        //signal(SIGUSR1, action);
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
        sleep(1);
        kill(pid /* child pid */, SIGUSR1);
        int status;
        int childPid = wait(&status);
        if (WIFEXITED(status)) {
            cout << "Parent: child process finished with pid = " << childPid << " and status = " << WEXITSTATUS(status) << endl;
        } else {
            cout << "Parent: child process finished abnormally. status = " << status << endl;    
            cout << "Sigusr1 = " << SIGUSR1 << endl;
            if (WIFSTOPPED(status)) {
                cout << "Stopped signal number = " << WSTOPSIG(status) << endl;
            }
        }
    }

}
