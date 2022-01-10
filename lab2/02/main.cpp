#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>


/*
    2. Кто выполняется первым после fork: отец или сын? Написать программу, которую можно использовать для проверки
    данной ситуаци.

    Первым выполняется дочерний процесс, а только затем уже родительский.
*/

int main() {    
    int x = 1234;
    int pid = fork();
    printf("current pid is %d\n", pid);
    
    if (pid == -1) {
        perror("Fork failed\n");
        exit(1);
    } else if (pid == 0) {
        printf("child pid %d\n", pid);
        exit(0);
    } else {
        int status;
        printf("parent pid %d\n", pid);
        wait(&status);
        printf("parent finished pid %d\n", pid);
        exit(0);
    }
}
