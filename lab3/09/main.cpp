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
    9. Включить в порожденный процесс вместо системного вызова kill системный вызов alarm с перехватом и без перехвата
    сигнала. Что изменится?
*/

void action(int sig) {
    cout << "SIGALRM comes in child process" << endl;
    sleep(1);
}

bool doAction = false;

int main() {
    int pid = fork();
    if (pid == 0) {
        cout << "Child: Pid = " << getpid() << endl;
        if (doAction) {
            signal(SIGALRM, action);
        }
        //alarm(2); // ignore b.o. next alarm signal
        alarm(6); // activate in 6 seconds

        for (int i = 1; i <= 100; i++) {
            int tmp = 0;
            for (int j = 1; j <= 5e7; j++) {
                tmp ^= j;
            }
            cout << "Child: loop index = " << i << endl;
        }

        exit(12);
    } else if (pid > 0) {
        cout << "Parent: Pid = " << getpid() << endl;
        sleep(1);
        int status;
        int childPid = wait(&status);
        if (WIFEXITED(status)) {
            cout << "Parent: child process finished with pid = " << childPid << " and exit code = " << WEXITSTATUS(status) << endl;
            cout << "status = " << status << endl;
        } else {
            cout << "Parent: child process finished abnormally " << childPid << " status = " << status << endl;
            cout << "SIGALRM = " << SIGALRM << endl;
        }
    }

}
