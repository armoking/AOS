#include <bits/stdc++.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
    8. Открыть файл (достаточно большого размера), доступный для чтения. Имя файла передается из командной строки. После
    открытия файла создать параллельный процесс с помощью только fork. В обоих процессах создать свои файлы для записи,
    читать информацию из общего файла и копировать ее в собственные выходные файлы (не на экран). Вывести на экран
    содержимое полученных файлов из порожденного процесса по окончании записи в файл и из родительского процесса,
    дождавшись завершения порожденного процесса. Посмотреть, что изменится, если читаемую процессами информацию сразу
    выводить на экран.

*/

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

int MODE = 0;

int main(int argc, char* argv[]) { 
    int fd = open(argv[1], O_RDONLY);
    int pid = fork();
    char buff[1 << 10];

    if (pid == -1) {
        cerr << "Fork failed" << endl;
        exit(1); // выход из родительского процесса
    } else if (pid == 0) {
        cout << "Child: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        int cfd = creat("./text_child.txt", 0777);
        
        int totalCount = 0;
        while (true) {
            int count = read(fd, buff + totalCount, 3);
            write(cfd, buff + totalCount, count);
            if (!MODE) {
                for (int x = 0; x < count; x++) {
                    cout << buff[totalCount + x];
                }
                cout.flush();
            }
            totalCount += count;
            if (count < 3) break;
        }
        close(cfd);
        
        if (MODE) {
            cout << "Child, data:";
            for (int i = 0; i < totalCount; i++) {
                cout << buff[i];
            }
            cout << endl;
        }
        exit(0);
    } else {
        cout << "Parent: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        int cfd = creat("./text_parent.txt", 0777);
        
        int totalCount = 0;
        while (true) {
            int count = read(fd, buff + totalCount, 3);
            write(cfd, buff + totalCount, count);
            if (!MODE) {
                for (int x = 0; x < count; x++) {
                    cout << buff[totalCount + x];
                }
                cout.flush();
            }
            totalCount += count;
            if (count < 3) break;
        }
        
        int status;
        wait(&status);   
        if (MODE) {
            cout << "Parent, data:";
            for (int i = 0; i < totalCount; i++) {
                cout << buff[i];
            }
            cout << endl;
        }
    }
    close(fd);

    
    

}
