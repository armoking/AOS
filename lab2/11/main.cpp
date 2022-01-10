#include <bits/stdc++.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
    11. Выполнить из программы на Си какую-либо команду Shell (cp или ls): с помощью вызовов fork-exec, с помощью
    библиотечной функции system. Необходимые для команды данные передать через аргументы командной строки.
*/

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char* argv[]) {
    int pid = fork();
    
    if (pid == -1) {
        cerr << "Fork failed" << endl;
        exit(1); // выход из родительского процесса
    } else if (pid == 0) {
        cout << "Child: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        argv++;
        execvp(argv[0], argv);
        exit(0);
    } else {
        cout << "Parent: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        std::string s;
        for (int i = 1; i < argc; i++) {
            if (i > 1) {
                s += " ";
            }    
            s += std::string(argv[i]);
        }
        system(s.c_str());

        int status;
        wait(&status);
        exit(0);
    }

    
        

}
