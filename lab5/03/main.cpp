#include <bits/stdc++.h>

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/wait.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

using namespace std;

/*
    2. Написать программу, позволяющую читать сообщения из очереди и выводить их на экран.
    Идентификатор очереди и тип сообщения передается через аргументы командной строки.
*/


const int MAX_BUFFER_SIZE = 1024;

struct TMessageBuffer {
    long type;               // 4 bytes
    char buffer[MAX_BUFFER_SIZE + 1];   // at most - 1024 bytes
    size_t messageSize;             // 8 bytes

    TMessageBuffer(string s = "", long type = 0) : type(type) {
        memset(buffer, 0, sizeof(buffer)); // just in case
        if (type == 0) {
            s = {};
        } else if (s.size() > MAX_BUFFER_SIZE) {
            cerr << "Message resized to MAX_BUFFER_SIZE - 1024 chars" << endl;
            s.resize(MAX_BUFFER_SIZE);
        }
        memcpy(buffer, s.data(), s.size());
        messageSize = s.size();    
    }
        
};


ostream& operator << (ostream& os, const TMessageBuffer& msg) {
    os << "type = " << msg.type << '\n';
    os << "buffer = " << msg.buffer << '\n';
    os << "msize = " << msg.messageSize << '\n';
    return os; 
}

void SendMessage(int queueId, TMessageBuffer msg) {
    struct msgbuf* msgbuff = reinterpret_cast<struct msgbuf*>(&msg);
    int response = msgsnd(queueId, msgbuff, msg.messageSize, msg.type);
     
    if (response != 0) {
        perror("Error while sending message to queue");
    }
}

struct TRecieveOpts {
    bool noWaitFlag = false;
    bool noErrorFlag = false;
    int maxSize = MAX_BUFFER_SIZE;
    int GetFlags() {
        int res = 0;
        if (noWaitFlag) {
            res |= IPC_NOWAIT;
        }
        if (noErrorFlag) {
            res |= MSG_NOERROR;
        }
        return res;
    }
};

TMessageBuffer RecieveMessage(int queueId, int messageType, TRecieveOpts opts = {}) {
    TMessageBuffer message;

    int response = msgrcv(queueId, reinterpret_cast<struct msgbuf*>(&message), opts.maxSize, messageType, opts.GetFlags());
    if (response >= 0) {
        message.messageSize = response;
    } else {
        perror("Error while reading message from queue");
    }
    
    return message;
}


void DeleteQueue(int queueId) {
    int response = msgctl(queueId, IPC_RMID, nullptr);
    if (response < 0) {
        perror("Error while deleting queue");
    }
}

struct msqid_ds GetAndLogQueueState(int queueId) {
    struct msqid_ds ds;
    int response = msgctl(queueId, IPC_STAT, &ds);
    cout << "=======================================" << endl;
    if (response < 0) {
        perror("Error while deleting queue");
    } else {
        cout << "Message state: (queueId = " << queueId << ")"  << endl;
        cout << "Owner's uid: " << ds.msg_perm.uid << endl;
        cout << "Owner's GID: " << ds.msg_perm.gid << endl;
        cout << "Message stime (last send time): " << ds.msg_stime << endl;
        cout << "Message rtime (last read time): " << ds.msg_rtime << endl;
        cout << "Message ctime (last attrs change time): " << ds.msg_ctime << endl;
        cout << "Number of messages: " << ds.msg_qnum << endl;
        cout << "MaxSize of queue: " << ds.msg_qbytes << endl;
    }
    cout << "=======================================" << endl;
    return ds;
}

void ParseFailed() {
    cout << "Parsing of args failed" << endl;
    exit(2);
 }


int main(int argc, char* argv[]) {
    // message sender
    // message receiver
    // queue deleter
    // queue creator
    if (argc < 2) {
        ParseFailed();
    }
    
    string t(argv[1]);
    if (t == "createQueue") {
        int queueId = msgget((key_t)1234, IPC_CREAT | 0660);
        cout << "ID of created queue: " << queueId << endl;
        exit(0);
    }


    if (argc < 3) {
        ParseFailed();
    }

    int queueId = -1;
    try {
        queueId = atoi(argv[2]);
    } catch (...) {
        ParseFailed();
    }   

    // queueId
    if (t == "deleteQueue") {
        DeleteQueue(queueId);
        cout << "Queue deleted called" << endl;
        exit(0);
    }
    
    if (argc < 4) {
        ParseFailed();
    }
    
    long type = -1;
    try {
        type = atol(argv[3]);
    } catch (...) {
        ParseFailed();
    }

    // queueId, type
    if (t == "getMessage") {
        TRecieveOpts opts;
        if (argc >= 5) {
            for (int i = 4; i < argc; i++) {
                string flag(argv[i]);
                if (flag == "nowait") {
                    opts.noWaitFlag = true;
                } else if (flag == "noerror") {
                    opts.noErrorFlag = true;
                    if (i + 1 < argc) {
                        string len(argv[i + 1]);
                        try {
                            opts.maxSize = stoi(len);
                        } catch (...) {
                            ParseFailed();
                        }
                    } else {
                        ParseFailed();
                    }
                }
            
            }
        }
        cout << "Trying get message with type: " << type << endl;
        auto message = RecieveMessage(queueId, type, opts);
        if (message.type > 0) {
            cout << "Recieved message: " << message.buffer << endl;
        }
        exit(0);
    }
    
    char* buff = nullptr;
    if (argc < 5) {
        ParseFailed();
    }
    buff = argv[4];

    // queueId, type, message
    if (t == "sendMessage") {
        SendMessage(queueId, TMessageBuffer(string(buff), type));
        cout << "Message has been sent" << endl;
        exit(0);
    }

    ParseFailed();
}


