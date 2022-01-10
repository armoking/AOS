#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <fcntl.h>

using namespace std;

/*
    7. Создать два параллельных процесса, в каждом из которых осуществляется работа с одним и тем же файлом.
    Каждый процесс открывает файл (имя файла передается через список аргументов командной строки).
    Один процесс пишет в файл, другой - читает из него.
    Что произойдет без включения блокировок?
*/

int main(int argc, char* argv[]) {
    
    {
        // fill file with letters *
        int fd = creat(argv[1], 0777);
        string s(1024 * 16, '*');
        write(fd, s.data(), s.size());
        close(fd);
    }

    auto filename = argv[1];
    int pid = fork();

    if (pid == 0) {
        int fd = open(filename, O_WRONLY);
        if (fd < 0) {
            cout << "Child process: file creation failed" << endl;
            exit(2);
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
        }

        close(fd);
        exit(0);

    } else if (pid > 0) {
        int fd = open(filename, O_RDONLY);
        
        if (fd < 0) {
            cout << "Parent process: file openning failed" << endl;
            exit(3);
        }

        char buffer[1024];
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

