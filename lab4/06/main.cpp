#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

using namespace std;

/*
    6. Создать именованный канал (любым способом). Убедиться, что канал создан.
    Рассмотреть особенности работы с именованным каналом (открыть файл с установленным и
    не установленным флагом O_NDELAY). Использовать его для обмена информацией с процессом,
    созданным другим пользователем (своим соседом).
*/

const char* FIFO = "armoking's_FIFO";

int main() {
    unlink(FIFO);
    int fifo = mkfifo(FIFO, 0777);

    if (fifo < 0) {
        cout << "FIFO creation failed" << endl;
        exit(1);
    }

    //int rfd = open(FIFO, O_RDONLY | O_NDELAY);
    int rfd = open(FIFO, O_RDONLY);
    if (rfd < 0) {
        cout << "FIFO openning failed" << endl;
        exit(1);
    }
    cout << "First fd = " << rfd << endl;
    while (true) {
        char buff[256];
        int len = read(rfd, buff, 256);
        if (len == 0) break;
        cout << "Armoking read: " << string(buff, buff + len) << endl;
    }
    close(rfd);
    cout << "Armoking process stopped" << endl;
}
