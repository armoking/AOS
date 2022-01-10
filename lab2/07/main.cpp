#include <bits/stdc++.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
    7. Модифицировать программу, включив в нее setpgrp в порожденный процесс до того, как он будет выполнять pause.
    Повлияет ли на порожденный процесс нажатие клавиши прерывания в тот момент, когда родительский процесс "спит"? Будет
    ли показан порожденный процесс в выводе команды ps?

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
        int spgrp = setpgrp();
        cout << "Child: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        cout << "Child: spgrp = " << spgrp << endl;
        pause();
        cout << "Child: process finished" << endl;
    } else {
        cout << "Parent: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        pause();
        cout << "Parent: process finished" << endl;
        exit(0);
    }
}
