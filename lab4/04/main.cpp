#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

using namespace std;

/*  
    4. Повторить п. 2, создавая параллельные процессы с помощью связки fork - exec. Как передать дескрипторы канала в порожденный процесс?
*/


class Process {
private:
    string name;
public:
    Process(string name) : name(name) {
        cout << name << ": process has started; pid = {" << getpid() << "};" << endl; 
    }

    ~Process() {
        cout << name << ": process has finished; pid = {" << getpid() << "};" << endl;
    }
};


void ParentProcess(int childPid, int* fd) {
    Process p("Parent");
    close(fd[1]);
    
    while (true) {
        char buff[1];
        int len = read(fd[0], buff, 1);
        if (len == 0) break;
        cout << "Parent read byte: " << buff[0] << endl;
    }
    
    close(fd[0]);
}


void ChildProcess(int* fd) {
    Process p("Child");
    dup2(fd[1], 1);
    close(fd[1]);
    execl("main2", "main2", NULL);
    close(fd[0]);
}

int main() {
    int fd[2];

    if (pipe(fd) != 0) {
        perror("Pipe openning failed");
        exit(-1);
    }

    int pid = fork();
    if (pid == 0) {
        ChildProcess(fd);
    } else {
        ParentProcess(pid, fd);
        int status;
        wait(&status);
    }

}
