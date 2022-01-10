#include <bits/stdc++.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
    10. Создать (с помощью связки fork - exec) параллельный процесс. Имя исполняемого файла для exec передается с помощью
    аргумента командной строки. Передать в порожденный процесс некоторую информацию через список параметров (список
    аргументов в функции main). Каждый процесс (и родительский, и порожденный) должен вывести на экран список переданных ему
    аргументов и свое окружение.

*/

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

#define env environ

extern char **env;

int main(int argc, char* argv[]) {
    int pid = fork();

    if (pid == -1) {
        cerr << "Fork failed" << endl;
        exit(1); // выход из родительского процесса
    } else if (pid == 0) {
        cout << "Child: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        
        execl(argv[1], argv[1], "first parameter", "second parameter", NULL);

        exit(0);
    } else {
        cout << "Parent: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        
        cout << "Parent arg list:" << endl;
        for (int i = 1; i < argc; i++) {
            cout << argv[i];
            if (i + 1 < argc) {
                cout << ", ";
            }
        }
        cout << endl;
    
        cout << "Parent env:" << endl;
        for (int i = 0; env[i] != NULL; i++) {
            cout << env[i] << ", ";
        }
        cout << endl;
        cout << "===========" << endl;
    
        int status;
        wait(&status);
        exit(0);
    }

    
        

}
