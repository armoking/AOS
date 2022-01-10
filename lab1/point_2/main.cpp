#include "../../common/mylib.h"

int main() {
    char fileName[50];
    printf("filename:\n");
    scanf("%s", fileName);

    int mode = 0;
    printf("mode (0400 - ro):\n");
    scanf("%i", &mode);

    int fd;
    if ((fd = creat(fileName, mode)) == -1) {
        perror("[line 14] Create file");
    } else {  
        int i;
        for (i = 0; i < 5; i++) {
            char* line = "Line number %d\n";
            char buffer[50];
            sprintf(buffer, line, i + 1);
            int code;

            if ((code = write(fd, buffer, strlen(buffer))) == -1) {
                perror("[line 24] Error while writting in file");
            } else {
                printf("length %d\n", code);
            }
        }
        close(fd); 
    }

    if ((fd == open(fileName, O_RDONLY)) == -1) {
        perror("[line 33] RDONLY permission error");
    } else {
        const int bufferSize = 1024;
        char buffer[bufferSize];
        int code;
        if ((code = read(fd, buffer, bufferSize)) == -1) {
            perror("[line 39] Error while reading file");
        } else {
            printf("%s\n", buffer);
        }
        close(fd); 
    }
    
    if ((fd = open(fileName, O_RDWR)) == -1) {
        perror("[line 47] RDWR permission error");
    } else {
        const int bufferSize = 1024;
        char buffer[bufferSize];
        int code;
        if ((code = read(fd, buffer, bufferSize)) == -1) {
            perror("[line 53] Error while reading file");
        } else {
            printf("%s\n", buffer);
        }
        close(fd);
    }
}
