#include "../../common/mylib.h"

using namespace std;


const bool with_times = false;

string get_short(string s) {
    string res = "";
    int cnt = 0;
    for (char ch : s) {
        res.push_back(ch);
        if (ch == '\n') {
            cnt++;
        }
        if (cnt == 10 + (with_times ? 3 : 0)) {
            break;
        }
    }
    return res;
}

int main() {
    struct stat s[1];
    vector<pair<string, string>> file_names = {
        {"dir", "./directory"},
        {"byte", "/dev/tty"},
        {"block", "/dev/vda"},
        {"text", "./main.txt"}
    };

    for (auto p : file_names) {
        string fileType = p.first;
        string fileName = p.second;
        stat(fileName.c_str(), s);
        cout << "(" << fileType << ") " << fileName << ":\n" << get_short(stat_to_string(*s)) << endl;
    }
}
