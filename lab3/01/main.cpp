#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>

using std::cin;
using std::cout;
using std::endl;
using std::cerr;

/*
    1. Изменить реакцию процесса на некоторый сигнал при помощи signal. В функции обработчике переустановить реакцию на
    сигнал на значение по умолчанию. Запустить программу и объяснить наблюдаемый результат.
*/

void action(int sig) {
    cerr << "\n" << "signal SIGINT" << endl;
}

int main() {
    signal(SIGINT, action);
    getchar();
}
