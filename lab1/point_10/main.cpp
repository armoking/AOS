#include "../../common/mylib.h"

using namespace std;

int main(int argc, char **argv) {    
    if (argc != 2) {
        errno = 5;
        Logger("Error while parsing parameters");
        exit(errno);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        Logger("Error while openning file1");
        exit(errno);
    }

    long long start = lseek(fd, 0, SEEK_SET);
    long long end = lseek(fd, 0, SEEK_END);

    for (auto i = end; i >= start - 1; i--) {
        CodeString str = readString(fd, 1);
        cout << str.str;
        lseek(fd, i, SEEK_SET);
    }

    cout << endl;
    close(fd);
}
