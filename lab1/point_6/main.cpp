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
        int i = 0;
        char* line = "Line number %d\n\n";
        char buffer[50];
        sprintf(buffer, line, i + 1);
        int code;
        
        int index = lseek(fd, -1, SEEK_CUR);
        printf("current index %d\n", index);

        if ((code = write(fd, buffer, strlen(buffer))) == -1) {
            perror("[line 24] Error while writting in file");
        } else {
            lseek(fd, 100000, SEEK_END);
        }
        
        if ((code = write(fd, buffer, strlen(buffer))) == -1) {
            
        }
        close(fd); 
    }

    if ((fd = open(fileName, 0400)) == -1) {
        perror("[line 37] Open file");
    } else {  
        int code;
        const int bufferSize = 102048;
        char buffer[bufferSize];
        int i;
        for (i = 0; i < 5; i++) {
            if ((code = read(fd, buffer, bufferSize)) == -1) {
                perror("[line 44] Error while reading file");
            } else {
                buffer[code] = '\0';
                printf("len = %d\n", code); 
                printf("data:\n%s\n", buffer);
                int j;
            }
            lseek(fd, 5, SEEK_CUR);
        }
        close(fd); 
    }
       

}
