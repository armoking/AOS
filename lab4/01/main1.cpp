#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

using namespace std;

/*
1. Создать параллельный процесс, используя только fork.
 Организовать обмен информацией между процессами через каналы.
 Предусмотреть однонаправленную и двунаправленную связь.
 При создании двунаправленной связи исключить возможность появления «смертельных объятий».    
*/


struct Process {
    string name;
    Process(string name) : name(name) {
        cout << name << ": process has started; pid = {" << getpid() << "};" << endl; 
    }

    ~Process() {
        cout << name << ": process has finished; pid = {" << getpid() << "};" << endl;
    }

};

struct IO {
    int fdIn, fdOut;
    IO(int* fd)
        : fdIn(fd[0])
        , fdOut(fd[1])
    {}
    
    string readBytes(int n, string name = "") {
        string res(n, '*');
        int len = read(fdIn, res.data(), n);
        res.resize(len);
        if (!name.empty()) {
            cout << name << " recieved string: " << res << endl;
        }
        return res;
    }

    int writeBytes(const string& s, string name = "") {
        int n = s.size();
        if (!name.empty()) {
            cout << name << " sending string: " << s << endl;
        }
        return write(fdOut, s.data(), n);
    }
};


void ParentProcess(int childPid, IO io) {
    Process parent("Parent");
    close(io.fdOut);
    
    auto s = io.readBytes(15, parent.name);

    close(io.fdIn);
}


void ChildProcess(IO io) {
    Process child("Child");
    close(io.fdIn);
    
    string tmp = "Hello, parent\n";
    io.writeBytes(tmp, child.name);

    close(io.fdOut);
}

int main() {
    int fd[2];
    if (pipe(fd) != 0) {
        perror("Pipe openning failed");
        exit(-1);
    }
    IO io(fd);
    int pid = fork();
    if (pid == 0) {
        ChildProcess(io);    
    } else {
        if (pid < 0) {
            throw;
        } else {
            ParentProcess(pid, io);
        }
    }
}

