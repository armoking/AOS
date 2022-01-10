#include "../../common/mylib.h"


int main() {
    char fileName[50];
    printf("filename:\n");
    scanf("%s", fileName);

    int mode = 0;
    printf("mode (0700 - rw):\n");
    scanf("%i", &mode);

    int fd;
    if ((fd = creat(fileName, mode)) == -1) {
        perror("[line 14] Create file");
    } else {  
        int i;
        for (i = 0; i < 5; i++) {
            char* line = "Line number %d\n\n";
            char buffer[50];
            sprintf(buffer, line, i + 1);
            int code;
            
            int index = lseek(fd, -1, SEEK_CUR);
            printf("current index %d\n", index);

            if ((code = write(fd, buffer, strlen(buffer))) == -1) {
                perror("[line 24] Error while writting in file");
            } else {
                lseek(fd, 0, SEEK_END);
            }
        }
        close(fd); 
    }

    if ((fd = open(fileName, 0400)) == -1) {
        perror("[line 37] Open file");
    } else {  
        int code;
        const int bufferSize = 128;
        char buffer[bufferSize];
        
        if ((code = read(fd, buffer, bufferSize)) == -1) {
            perror("[line 44] Error while reading file");
        } else {
            printf("data:\n%s\n", buffer);
        }
        close(fd); 
    }
       

}
