#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

using namespace std;

/*  
    5. С помощью системных вызовов pipe и dup реализовать конвейер: who | wc -l.
*/


class Process {
private:
    string name;
public:
    Process(string name) : name(name) {
        cout << name << ": process has started; pid = {" << getpid() << "};" << endl; 
    }
};


void ParentProcess(int childPid, int* fd) {
    Process p("Parent");
    close(fd[1]);   
    
    dup2(1, fd[1]);
    dup2(fd[0], 0);
    execlp("wc", "wc", "-l", NULL);
    close(fd[0]);
}


void ChildProcess(int* fd) {
    Process p("Child");
    close(fd[0]);
    dup2(fd[1], 1);
    execlp("who", "who", NULL);
    close(fd[1]);
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
