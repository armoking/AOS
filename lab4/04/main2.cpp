#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

using namespace std;

/*  
    4. Повторить п. 2, создавая параллельные процессы с помощью связки fork - exec. Как передать дескрипторы канала в порожденный процесс?
*/

int main(int argc, char* argv[]) {
    char buff[256];
    int len = read(0, buff, sizeof(buff));
    write(1, buff, len);
}
