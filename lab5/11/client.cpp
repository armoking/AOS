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

bool SendMessage(int queueId, TMessageBuffer msg) {
    struct msgbuf* msgbuff = reinterpret_cast<struct msgbuf*>(&msg);
    int response = msgsnd(queueId, msgbuff, msg.messageSize, msg.type);
    if (response < 0) {
        return false;
    }
    if (response != 0) {
        perror("Error while sending message to queue");
    }
    return true;
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
        message.type = -1;
        return message;
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
    if (response < 0) {
        perror("Error while getting queue stat");
    }
    return ds;
}

const string MESSAGE_FROM_USER = "user-message:";
const string INFO_MESSAGE = "user-info:";

enum MESSAGE_TYPES {
    INFO_MESSAGE_TYPE = 1,
    USER_MESSAGE_TYPE = 2,
    USER_LIST_TYPE = 3
};


string CreateInfoMessage(int pid, int qid, int start) {
    string req = INFO_MESSAGE + to_string(pid) + string(",") + to_string(qid) + string(",") + to_string(start); 
    return req;
}

int GetServerQueueId() {
    key_t key = ftok("armoking_server_queue", 123); // for server queue id
    int queueId = msgget(key, 0660);  
    while (queueId < 0) {
        sleep(1);
        queueId = msgget(key, 0660);  
    }
    return queueId; 
}

void SendInfoMessageToServer(int serverQueueId, int clientQueueId, bool start) {
    string infoMessage = CreateInfoMessage(getpid(), clientQueueId, start);
    SendMessage(serverQueueId, TMessageBuffer(infoMessage, INFO_MESSAGE_TYPE)); // тип сообщения равен 1, так как это "системное" сообщение с наивысшим приоритетом
}

int clientQueueId;
int serverQueueId;

void action(int sig) {
    SendInfoMessageToServer(serverQueueId, clientQueueId, false);
    cout << "Client (" << getpid() << ") stopped" << endl;
    exit(0);
}

bool StartsWith(string s, string t) {
    return s.size() >= t.size() && s.substr(0, t.size()) == t;
}

int calculate() {
    return 1;
}

// client - side
int main(int argc, char* argv[]) {
    signal(SIGINT, action);

    serverQueueId = GetServerQueueId();
    clientQueueId = serverQueueId;
    SendInfoMessageToServer(serverQueueId, clientQueueId, true);
    
    cout << "Client (" << getpid() << ") queueID = " << clientQueueId << endl; 
    
    long long value = 0;
    for (int itr = 0; itr < 30; itr++) {
        int clientQueueSize = GetAndLogQueueState(clientQueueId).msg_qnum;
        bool used = false;
        if (clientQueueSize != 0) {
            TRecieveOpts opts;
            opts.noWaitFlag = true;
            while (true) {
                TMessageBuffer msg = RecieveMessage(clientQueueId, getpid(), opts);
                if (msg.type == -1) {
                    break;
                } else {
                    cout << "Client (" << getpid() << ") recieved message with type = " << msg.type << ". Message = " << msg.buffer << endl;
                    used = true;
                    string s(msg.buffer);
                    s.pop_back();
                    int len = s.size();
                    int pos = len - 1;
                    while (pos >= 0 && s[pos] != '{') {
                        pos--;
                        len++;
                    }
                    long long prefValue = stoll(s.substr(pos + 1, len - 1));
                    value += prefValue;
                }
            }  
        }

        if (!used) {
            value += calculate();
            // message to any client, that server will chose
            string s = MESSAGE_FROM_USER + string("any,")
                + to_string(getpid()) + string(",") 
                + string("Value from client (") + to_string(getpid()) + string(") {") + to_string(value) + string("}");
            if (!SendMessage(serverQueueId, TMessageBuffer(s, 2))) {
                cout << "Client (" << getpid() << ") has no access to servers queue. Client will be stopped" << endl;
                break;
            }
        }
    
        sleep(1);
    }

    action(0);
    return 0;
}
