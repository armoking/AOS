#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <fcntl.h>

using namespace std;

/*
    8. Добавить в один из процессов использование слабой блокировки. Что 
    произойдет?
*/

bool useFlock = true;

int main(int argc, char* argv[]) {
    
    {
        // fill the file with symbols '*'
        int fd = creat(argv[1], 0777);
        string s(1024 * 16, '*');
        write(fd, s.data(), s.size());
        close(fd);
    }

    auto filename = argv[1];
    int pid = fork();

    if (pid == 0) {
        int fd = open(filename, O_RDWR);
        if (fd < 0) {
            cout << "Child process: file creation failed" << endl;
            exit(2);
        }    
        
        int fcl = -1; 
        struct flock l;
        
        if (useFlock) {
            fcl = flock(fd, LOCK_UN);
        } else {
            l.l_type = F_RDLCK;
            l.l_whence = SEEK_SET;
            l.l_start = 0;
            l.l_len = 16 * 1024; // until end of the file

            int fcl = fcntl(fd, F_SETLKW, &l);
            if (fcl < 0) {
                cout << "Child process: fcntl set lock on file failed" << endl;
                perror("Here:");
                exit(2);
            } else {
                cout << "Child process: lock set successfully" << endl;
            }
        }

        for (int i = 0; i < 15; i++) {
            string str(1023, char('a' + i));
            str += "\n";
            int len = write(fd, str.data(), str.size());
            if (len < 0) {
                cout << "Child process: operation writting to file failed" << endl;
                kill(getppid(), SIGKILL);
                exit(2);
            }
            cout << "Child " << i << endl;
        }

        if (useFlock) {
            fcl = flock(fd, LOCK_UN);
        } else {
            l.l_type = F_UNLCK;
            fcl = fcntl(fd, F_SETLKW, &l);
        }
        close(fd);
        exit(0);

    } else if (pid > 0) {
        
        int fd = open(filename, O_RDONLY);
        if (fd < 0) {
            cout << "Parent process: file openning failed" << endl;
            exit(3);
        }
        
        char buffer[512];
        int index = 0;
        while (true) {
            int len = read(fd, buffer, sizeof(buffer));
            if (len < 0) {
                cout << "Parent process: read error!" << endl;
                kill(pid, SIGKILL);
                exit(3);
            } else if (len == 0) {
                break;
            }
            cout << "Parent read: " << string(buffer, buffer + len) << endl;
        } 

        close(fd);

        int status;
        wait(&status);
        
        exit(0);
    } else {
        cout << "Fork failed" << endl;
        exit(1);
    }
} 

