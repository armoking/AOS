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


int main(int argc, char* argv[], char* envp[]) {

    cout << "Child of child: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
    
    cout << "Child arg list: " << endl;
    for (int i = 1; i < argc; i++) {
        cout << argv[i];
        if (i + 1 < argc) cout << ", ";
    }
    cout << endl;

    cout << "Child env:" << endl;
    for (int i = 0; env[i] != NULL; i++) {
        cout << env[i] << ", ";
    }
    cout << endl;
    cout << "===========" << endl;

}
