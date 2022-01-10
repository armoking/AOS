#include <bits/stdc++.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
    4. Изменить программу п. 3 так, чтобы родительский процесс завершился раньше порожденного. Какой процесс становится
    родительским для порожденного после того, как исходный родительский процесс завершился?
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
        sleep(3);
        cout << "Child: (after end of parent) Pid = " << getpid()
            << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        cout << "Child: process finished" << endl;
        exit(0);
    } else {
        cout << "Parent: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        cout << "Parent: process finished" << endl;
        exit(0);
    }
}
