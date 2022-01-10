#include <bits/stdc++.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
    3. Написать программу, в которой процесс порождает новый и позволяет порожденному процессу завершиться. Оба процесса
    должны выводить свои идентификаторы (процесса, родительского процесса, группы процессов). Проверить идентификатор и
    статус завершения порожденного процесса. Выполнить данную программу и посмотреть ее результаты.
*/

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

bool child_process_finished_normally = false;

int main() {
    int pid = fork();
    
    if (pid == -1) {
        cerr << "Fork failed" << endl;
        exit(1); // выход из родительского процесса
    } else if (pid == 0) {
        cout << "Child: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        if (child_process_finished_normally) {
            exit(123);    
        } else {
            throw;
        }
    } else {
        cout << "Parent: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        int status;
        int childPid = wait(&status);
        cout << "Parent: child process " << childPid << " has been finished" << endl;
        auto childExitCode = WIFEXITED(status);
        if (childExitCode) {
            cout << "Parent: child exited with exit code = " << childExitCode << endl;
            cout << "Parent: child status = " << WEXITSTATUS(status) << endl;
        } else {
            cout << "Parent: child terminated abnomrally" << endl;
            cout << status << endl;
        }
        exit(0);
    }
}
