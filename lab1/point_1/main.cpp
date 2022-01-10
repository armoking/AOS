#include "../../common/mylib.h"

void printError() {
    perror("Print error with perror func");
    printf("errno = %d\n", errno);
    printf("sys_errlist[%d] = %s\n", errno, sys_errlist[errno]);
}

int main() {
    string filename = "file.txt";
    int fd = open(filename.c_str(), O_RDONLY | O_CREAT, 0);
    if (fd == -1) {
        printError();
        exit(0);
    }
    close(fd);
    fd = open(filename.c_str(), O_RDONLY, 0777);
    if (fd == -1) {
        printError();
        exit(0);
    } else {
        printf("Error not found\n");
        close(fd);
    }
}
