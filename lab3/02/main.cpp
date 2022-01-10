#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>

using std::cin;
using std::cout;
using std::endl;
using std::cerr;

/*
    2. Модифицировать программу п.1 так, чтобы вместо signal использовать sigaction.
    https://www.opennet.ru/man.shtml?topic=sigaction&category=2&russian=0
*/

void action(int sig) {
    cout << "\n" << "signal SIGINT" << endl;   
    alarm(5);
    sleep(15);
    cout << "End " << endl;
}

void action2(int sig) {
    cout << "Action 2 " << sig << endl;
}

int main() {
    struct sigaction act; 
    struct sigaction old;
    act.sa_handler = action; 
    sigemptyset(&act.sa_mask);     
       
    act.sa_flags = 0;
    sigaction(SIGINT, &act, &old);
   
    signal(SIGALRM, action2);
    
    getchar();
}
