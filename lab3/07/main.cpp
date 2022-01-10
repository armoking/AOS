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
    7. Повторить выполнение предыдущих пунктов задания, используя в порожденном процессе вместо вложенных циклов
    системный вызов pause. Что изменится? Как завершить процесс в случае выполнения с pause п. 4?
*/

bool childProcessFinished = false;

void action(int sig) {
    cout << "SIGUSR1 comes in child process" << endl;
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
        pause();
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
