#include <bits/stdc++.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
    5. Изменить программу п. 3 так, чтобы родительский процесс выполнялся, не ожидая завершения порожденного процесса.
    Что произойдет с порожденным процессом, когда он завершится? Как убить зомби?

    Порождённый процесс станет зомби. Если ничего не делать, то дочерний процесс завершиться после завершения
    родительского. Чтобы убить зомби можно убить родительский процесс.

    ps aux | grep -w Z
    ps ajx | grep -w Z

*/

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

int main() {
    int pid = fork();
    
    if (pid == -1) {
        cerr << "Fork failed" << endl;
        exit(1); // выход из родительского процесса
    } else if (pid == 0) {
        cout << "Child: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        cout << "Child: (after end of parent) Pid = " << getpid()
            << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        cout << "Child: process finished" << endl;
    } else {
        cout << "Parent: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        sleep(60);
        cout << "Parent: process finished" << endl;
        exit(0);
    }
}
