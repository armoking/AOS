#include "../../common/mylib.h"

using namespace std;

int main(int argc, char* argv[]) {
    int input_fd, output_fd;
    bool runtime = false;

    if (argc == 1) {
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

        dup2(input_fd, STDIN_FILENO);
        dup2(output_fd, STDOUT_FILENO);

    } else {
        errno = 5;
        Logger("Error while parsing arguments. Number of arguments must be 0 or 2, but " + to_string(argc - 1) + " were given");
        exit(errno);
    }
    
    const string copyFile = "copy";
    {
        struct stat s[1];
        int code = stat((copyFile + ".exe").c_str(), s);   
        if (code < 0) {
            cerr << "Compiling of " + copyFile + ".cpp" << endl;
            system(("g++ -std=c++14 " + copyFile + ".cpp -o " + copyFile + ".exe").c_str());
            cerr << "Compilation finished successfully" << endl;
        }
    }

    execl((copyFile + ".exe").c_str(), NULL);    

    close(input_fd);
    close(output_fd);
}
