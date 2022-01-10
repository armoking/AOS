#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

/*
    6. Создать именованный канал (любым способом). Убедиться, что канал создан.
    Рассмотреть особенности работы с именованным каналом (открыть файл с установленным и
    не установленным флагом O_NDELAY). Использовать его для обмена информацией с процессом,
    созданным другим пользователем (своим соседом).
*/

const char* FIFO = "armoking's_FIFO";

int main() {
    cout << "Second start" << endl;
    int wfd = open(FIFO, O_WRONLY | O_NONBLOCK);
    cout << "Second open FIFO" << endl;
    
    if (wfd < 0) {
        cout << "FIFO openning for write failed" << endl;
        exit(1);
    }
    
    string s = "Hello, there";
    write(wfd, s.data(), s.size());
    
    close(wfd);
    unlink(FIFO);
    exit(0);
}
