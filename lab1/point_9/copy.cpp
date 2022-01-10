#include "../../common/mylib.h"

using namespace std;


void copyFunc(int input_fd, int output_fd, bool runtime) {
    CodeString codeString;
    while ((codeString = readString(input_fd, runtime ? 1 : 1 << 20)).str.size()) {
        string buffer = codeString.str;
        if (write(output_fd, buffer.c_str(), buffer.size()) == -1) {
            Logger("Error while writing in file2");
            close(input_fd);
            close(output_fd);
            exit(errno);
        }   
    }

    if (codeString.error_code != 0) {
        errno = codeString.error_code;
        Logger("Error while reading file1");
        exit(codeString.error_code);
    }
}


int main(int argc, char* argv[]) {
    int input_fd, output_fd;
    bool runtime = false;

    if (argc == 1 || true /* point 9 */) {
        runtime = true;
        input_fd = STDIN_FILENO;
        output_fd = STDOUT_FILENO;
    } else if (argc == 3) {
        input_fd = open(argv[1], O_RDONLY);
        if (input_fd < 0) {
            Logger("Error while open file1");
            exit(errno);
        }
        
        struct stat s[1];
        stat(argv[1], s);
        
        output_fd = creat(argv[2], s[0].st_mode);
        if (output_fd < 0) {
            Logger("Error while creating file2");
            exit(errno);
        }
    } else {
        errno = 5;
        Logger("Error while parsing arguments. Number of arguments must be 0 or 2, but " + to_string(argc - 1) + " were given");
        exit(errno);
    }
    
    copyFunc(input_fd, output_fd, runtime);

    close(input_fd);
    close(output_fd);
}
