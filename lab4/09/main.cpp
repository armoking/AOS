#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <fcntl.h>

using namespace std;

/*
    9. Добавить использование слабой блокировки во второй. Что изменится?
*/

// settings:

bool useFlock = false;
bool firstLock = true;
bool secondLock = true;
int blocksNum = 27;

int main(int argc, char* argv[]) {
    {
        // fill the file with symbols '*'
        int fd = creat(argv[1], 0777);
        string s(blocksNum * 1024, '*');
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
        if (!firstLock) {
        } else if (useFlock) {
            fcl = flock(fd, LOCK_EX);
        } else {
            l.l_type = F_WRLCK;
            l.l_whence = SEEK_SET;
            l.l_start = 0;
            l.l_len = blocksNum * 1024; // until end of the file

            int fcl = fcntl(fd, F_SETLKW, &l);
            if (fcl < 0) {
                cout << "Child process: fcntl set lock on file failed" << endl;
                perror("Here:");
                exit(2);
            } else {
                cout << "Child process: lock set successfully" << endl;
            }
        }

        for (int i = 0; i < blocksNum - 1; i++) {
            string str(1023, char('a' + i));
            str += "\n";
            int len = write(fd, str.data(), str.size());
            if (len < 0) {
                cout << "Child process: operation writting to file failed" << endl;
                kill(getppid(), SIGKILL);
                exit(2);
            }
            cout << "Child wrote symbol " << char('a' + i) << endl;
        }

        if (!firstLock) {

        } else if (useFlock) {
            fcl = flock(fd, LOCK_UN);
        } else {
            l.l_type = F_UNLCK;
            fcl = fcntl(fd, F_SETLKW, &l);
        }
        close(fd);
        exit(0);

    } else if (pid > 0) {

        int fd = open(filename, O_RDWR);
        if (fd < 0) {
            cout << "Parent process: file openning failed" << endl;
            exit(3);
        }

    
        int fcl = -1;
        struct flock l;
        if (!secondLock) {
        } else if (useFlock) {
            fcl = flock(fd, LOCK_SH);
        } else {
            l.l_type = F_RDLCK;
            l.l_whence = SEEK_SET;
            l.l_start = 0;
            l.l_len = 16 * 1024; // until end of the file

            int fcl = fcntl(fd, F_SETLKW, &l);
            if (fcl < 0) {
                cout << "Parent process: fcntl set lock on file failed" << endl;
                perror("Here:");
                exit(2);
            } else {
                cout << "Parent process: lock set successfully" << endl;
            }
        }

        char buffer[256];
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
        if (!secondLock) {
        } else if (useFlock) {
            fcl = flock(fd, LOCK_UN);
        } else {
            l.l_type = F_UNLCK;
            fcl = fcntl(fd, F_SETLKW, &l);
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
