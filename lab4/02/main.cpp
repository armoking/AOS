#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

using namespace std;

/*  
    2. Модифицировать программу п. 1 таким образом, чтобы родительский процесс читал из канала в цикле,
    пока не встретится конец файла (порожденный процесс может записывать в канал информацию, читаемую им из стандартного входного потока).
    Как избежать бесконечного ожидания родительским процессом чтения из пустого канала?
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
        char buffer[1];
        int len = read(fd[0], buffer, 1);
        if (len) {
            if (buffer[0] == EOF) {
                cout << "Parent stopped" << endl;
                break;
            } else {
                cout << "Parent read symbol: '" << buffer[0] << "'" << endl; 
            }
        } else {
            break;
        }
    }

    close(fd[0]);
}


void ChildProcess(int* fd) {
    Process p("Child");
    close(fd[0]);
    
    char buff[256];
    int len = read(0, buff, sizeof(buff) - 1);
    cout << "Child read " << len << " characters." << endl;
    buff[len] = '\0';
    write(fd[1], buff, len + 1);
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
