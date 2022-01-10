#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <fcntl.h>

using namespace std;

/*
    11. Повторить выполнение п. 10, включив в процессы для синхронизации работы с терминалом использование режима слабой блокировки.
    Как изменится поведение процессов?
*/


void run(string name) {
    char buff[512];
    int iter = 0;    
    while (true) {
        cout << name << " trying to read " << endl;

        int len = read(0, buff, sizeof(buff));
        if (len < 0) {
            cout << name << " process failed while reading stdin" << endl;
            break;
        }
        
        if (name == "Parent") {
            sleep(1);
        }       
        
        string result(buff, buff + len);
        result = name + " process read: \"" + result;
        result.back() = '"';
        result.push_back('\n');

        cout << name << " trying to write " << endl;
        write(1, result.data(), result.size());      
        cout << name << " on iteration = " << (++iter) << endl;
    }
}

int main(int argc, char* argv[]) {
    int pid = fork();

    if (pid == 0) {
        run("Child");
        exit(0);
    } else if (pid > 0) {
        run("Parent");
        int status;
        wait(&status);
        exit(0);
    } else {
        cout << "Fork failed" << endl;
        exit(1);
    }
}
