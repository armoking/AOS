#include "../../common/mylib.h"

using namespace std;

long long getFileSize(char* fileName) {
    int fd = open(fileName, O_RDONLY);
    if (fd < 0) {
        Logger("Cannot open file " + string(fileName) + ".");
        return -1;
    } else {
        struct stat s[1];
        stat(fileName, s);
        return s[0].st_size;
    }
}

int main(int argc, char **argv) {
	if (argc == 1) {
        cout << "Empty list of files. Aborting..." << endl;;
        exit(0);
    }

    long long maxLength = -1;
    int answer = 0;
    for (int i = 1; i < argc; i++) {
        long long currentLength = getFileSize(argv[i]);
        if (currentLength > maxLength) {
            maxLength = currentLength;
            answer = i;
        }       
    }

    if (answer == 0) {
        cout << "Errors were throwen while reading all given files." << endl;
    } else {
        cout << "The bigget file is " << argv[answer] << endl;
        cout << "Byte size = " << maxLength << endl;
    }

}
