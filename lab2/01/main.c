#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define env environ

extern char **env;


int main() {    
    if (setenv("MYVAR", "value of my MYVAR", 0) != 0) {
        printf("setenv failed");
        exit(1);
    }

    printf("%s\n", getenv("MYVAR"));
    unsetenv("MYVAR");

    for (int i = 0; env[i] != NULL; i++) {
        printf("%s\n", env[i]);
    }
}
