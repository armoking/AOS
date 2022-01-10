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
    10. Написать программу, иллюстрирующую способ блокирования дополнительных сигналов на время работы обработчика сигнала. Что
    произойдет, если во время обработки некоторого сигнала в процесс поступит несколько однотипных заблокированных сигналов.
    
    Логика работы блокировщика сигналов: Если во время блокировки процессу было послано несколько одинаковых сигналов
    sig, то при разблокировании поступит только один. Поступление сигналов во время блокировки просто отмечается в
    специальной битовой шкале в паспорте процесса.
*/

void intAction(int sig) {
    cout << "int signal before sleep " << sig << endl;
    sleep(4);
    cout << "int signal after sleep " << sig << endl;
}

void simpleAction(int sig) {
    cout << "simple signal before sleep " << sig << endl;
    sleep(5);
    cout << "simple signal after sleep " << sig << endl;
}

int main() {
    static struct sigaction act, oldAct, actInt, oldIntAct;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT); // SIGINT теперь будет блокироваться
    
    actInt.sa_handler = intAction;
    actInt.sa_mask = mask;
    actInt.sa_flags = SA_RESTART;
    sigaction(SIGINT, &actInt, &oldIntAct);

    act.sa_handler = simpleAction;
    act.sa_mask = mask;
    act.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &act, &oldAct);
    
    int pid = fork();
    if (pid == 0) {
        cout << "Child: Pid = " << getpid() << endl;
        sleep(1);

        exit(12);
    } else if (pid > 0) {
        cout << "Parent: Pid = " << getpid() << endl;
        sleep(5);
        // sigaction(SIGCHLD, &oldAct, NULL); // меняем способ обработки сигнала на исходный

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
