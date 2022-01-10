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
    struct flock readLock, writeLock;
        
    auto resetLock = [&] (struct flock& l) { 
        l.l_type    = F_WRLCK;
        l.l_start   = 0;
        l.l_len     = 0;
        l.l_whence  = SEEK_CUR;
    };
    
    int iter = 0;    
    while (true) {
        resetLock(readLock);
        resetLock(writeLock); 

        int rid = fcntl(0, F_SETLKW, &readLock);
        int wid = fcntl(1, F_SETLKW, &writeLock);
        if (rid == -1 || wid == -1) {
            cout << name << " fcnt fail" << endl;
            continue;
        } else {
            cout << name << " successefully locked 0 1 " << endl;
        }
        
        if (name == "Parent") {
            sleep(1);
        }

        int len = read(0, buff, sizeof(buff));
        if (len < 0) {
            cout << name << " process failed while reading stdin" << endl;
            break;
        }
        
        string result(buff, buff + len);
        result = name + " process read: \"" + result;
        result.back() = '"';
        result.push_back('\n');

        write(1, result.data(), result.size());
        
        readLock.l_type = writeLock.l_type = F_UNLCK;
        rid = fcntl(0, F_SETLKW, &readLock);
        wid = fcntl(1, F_SETLKW, &writeLock);
        cout << name << " unlocked 0 1 " << endl;
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
