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

struct IO {
    int* fd;
    IO(int* fd) : fd(fd) {}
    
    string readBytes(string name = "") {
        string prefix;
        while (prefix.empty() || prefix.back() != '}') {
            char buffer[1];
            int l = read(fd[0], buffer, 1);
            if (l) {
                prefix.push_back(buffer[0]);
            }
        }
        if (prefix[0] != '{') {
            cout << "Failder to read header" << endl;
            assert(false);
        }

        int len = stoi(prefix.substr(1, prefix.size() - 2));
        char buffer[len];
        int cur = 0;
        while (cur < len) {
            cur += read(fd[0], buffer + cur, len - cur);
        }
        string result(buffer, buffer + cur);
        cout << name << " has recieved message: " << result << endl;
        return result;
    }

    void writeBytes(string s, string name = "") {
        int n = s.size();

        string prefix = "{" + to_string(n) + "}";
        s = prefix + s;     

        int cur = 0;
        while (cur < s.size()) {
            cur += write(fd[1], s.data() + cur, s.size() - cur);
        }
        cout << name << " has sent message: " << s << endl;
    }
};


void ParentProcess(int childPid, IO parentIO, IO childIO) {
    Process pr("Parent");
    close(parentIO.fd[0]);
    close(childIO.fd[1]);
    

    int numberOfQueries = 7;
    
    for (int i = 0; i < numberOfQueries; i++) {
        string stringForSend = "I want to send message with number " + to_string(i) + " to child.";
        parentIO.writeBytes(stringForSend, "parent"); 
        string s = childIO.readBytes("parent");
    }
    
    parentIO.writeBytes("close.", "parent");
    auto fromChild = childIO.readBytes("parent");
    
    close(parentIO.fd[1]);
    close(childIO.fd[0]);
}


void ChildProcess(IO parentIO, IO childIO) {
    alarm(20); // in case of infinite loops
    Process pr("Child");
    close(parentIO.fd[1]);
    close(childIO.fd[0]);
    
    int index = 0;
    while (true) {
        string s = parentIO.readBytes("child");
        ++index;
        if (s == "close.") {
            childIO.writeBytes("bye.", "child");
            break;
        } else {
            childIO.writeBytes("Got " + to_string(index) + "-th message from parent.", "child");
        }
    }

    close(parentIO.fd[0]);
    close(childIO.fd[1]);
}

int main() {
    int fd[2];
    int fd2[2];

    if (pipe(fd) != 0) {
        perror("Pipe openning failed");
        exit(-1);
    }
 
    if (pipe(fd2) != 0) {
        perror("Pipe 2 openning failed");
        exit(-1);
    }   

    IO io(fd);
    IO io2(fd2);
    int pid = fork();
    if (pid == 0) {
        ChildProcess(io, io2);    
    } else {
        ParentProcess(pid, io, io2);
        int status;
        wait(&status);
    }
}

