#include <bits/stdc++.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
    6. Изменить программу п. 3 так, чтобы оба процесса выполняли системный вызов pause после вывода идентификаторов.
    Запустить программу в фоновом режиме и посмотрите (с помощью ps), какие процессы существуют для данного
    пользователя. Выполнить kill -9 pid для родительского процесса. Что произойдет с порожденным? После очистки текущего
    сеанса, вновь выполнить программу, но уже в интерактивном режиме. Нажать на терминале клавишу CTRL-C. Что произойдет
    с родительским и порожденным процессами? Можно ли получить такой же результат, если нажать клавишу прерывания после
    того, как родительский процесс завершится?
    
    В случае запуска в фоновом режиме, оба процесса будут ждать сигналов.
    При вызове kill -9 pid - будет вызвал сигнал SIGKILL, который убьёт только данный процесс (безуслоное завершение)
    При CTRL-C генерирует сигнал SIGINT и отправляет его всем процессам (и родительскому, и дочернему), из-за чего и
    завершаются оба процесса.


*/

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

int main() {
    int pid = fork();
    
    if (pid == -1) {
        cerr << "Fork failed" << endl;
        exit(1); // выход из родительского процесса
    } else if (pid == 0) {
        cout << "Child: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        pause();
        cout << "Child: process finished" << endl;
    } else {
        cout << "Parent: Pid = " << getpid() << " Parent Pid = " << getppid() << " Group Pid = " << getpgid(getpid()) << endl;
        pause();
        cout << "Parent: process finished" << endl;
        exit(0);
    }
}
