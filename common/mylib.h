#include <bits/stdc++.h>

#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>


using namespace std;

struct CodeString {
    string str;
    int code;
    int error_code;
    CodeString(const string& str = "", int code = 0, int error_code = 0)
        : str(str)
        , code(code)
        , error_code(error_code)
    {
    }
};

string to8(long long x) {
    char buff[8];
    sprintf(buff,"%o", x);
    return string(buff);
}


string stat_to_string(struct stat s) {
    string res;
    res += "device: " + to_string(s.st_dev) + ";\n";
    res += "inode: " + to_string(s.st_ino) + ";\n";
    res += "access mode: " + to8(s.st_mode) + ";\n";
    res += "nlink: " + to_string(s.st_nlink) + ";\n";
    res += "uid: " + to_string(s.st_uid) + ";\n";
    res += "gid: " + to_string(s.st_gid) + ";\n";
    res += "rdev: " + to_string(s.st_rdev) + ";\n";
    
    res += "size: " + to_string(s.st_size) + ";\n";
    res += "block size: " + to_string(s.st_blksize) + ";\n";
    res += "blocks number: " + to_string(s.st_blocks) + ";\n";
    res += "atime: " + to_string(s.st_atime) + ";\n";
    res += "mtime: " + to_string(s.st_mtime) + ";\n";
    res += "ctime: " + to_string(s.st_ctime) + ";\n";
    return res;
}

CodeString readString(int fd, int len = 1 << 20) {
    string str(len, '*');
    int code = read(fd, &str[0], len);
    if (code >= 0) {
        str.resize(code);
    } else {
        str.resize(0);
    }
    return CodeString(str, code, errno);
}

struct Logger {
    Logger() {
        cerr << "[Logger] Errno=" << errno << " " << sys_errlist[errno] << endl; 
    }
    Logger(string s) {
        cerr << s << endl;
        cerr << "[Logger] Errno=" << errno << " " << sys_errlist[errno] << endl; 
    }
};
