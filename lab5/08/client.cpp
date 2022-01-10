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
    6. Написать программы для работы с очередями сообщений в соответствии с моделью клиент-сервер:
        - каждый процесс использует собственную очередь сообщений.
        - Процесс-сервер читает запросы из своей очереди сообщений и посылает ответ процессам-клиентам в их очереди.
        - Процессы-клиенты читают ответ и выводят его в выходной поток.
        - Процессы-клиенты должны передавать процессу-серверу информацию о своих очередях сообщений (куда записывать ответ).
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
    if (response < 0) {
        perror("Error while deleting queue");
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
        cout << "Client (" << getpid() << ") can't find server`s queue" << endl;
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
    DeleteQueue(clientQueueId);
    cout << "Client (" << getpid() << ") stopped" << endl;
    exit(0);
}

bool StartsWith(string s, string t) {
    return s.size() >= t.size() && s.substr(0, t.size()) == t;
}

// client - side
int main(int argc, char* argv[]) {
    signal(SIGINT, action);

    serverQueueId = GetServerQueueId();
    clientQueueId = msgget(getpid() + 512 /* random shift */, 0660 | IPC_CREAT);
    SendInfoMessageToServer(serverQueueId, clientQueueId, true);
    cout << "Client (" << getpid() << ") queueID = " << clientQueueId << endl; 
    
    
    for (int itr = 0; itr < 30; itr++) {
        int clientQueueSize = GetAndLogQueueState(clientQueueId).msg_qnum;
        if (clientQueueSize != 0) {
            for (int i = 0; i < clientQueueSize; i++) {
                TMessageBuffer msg = RecieveMessage(clientQueueId, 0);
                cout << "Client (" << getpid() << ") recieved message with type = " << msg.type << ". Message = " << msg.buffer << endl;
                
            }
        } else {
            if (rand() & 1) {
                // message to everyone
                string s = MESSAGE_FROM_USER + string("all,")
                    + to_string(getpid()) + string(",")
                    + string("Hello from client ") + to_string(getpid()) + string("hsh [") + to_string(rand() % 10) + string("]!!!");
                if (!SendMessage(serverQueueId, TMessageBuffer(s, getpid()))) {
                    cout << "Client (" << getpid() << ") has no access to servers queue. Client will be stopped" << endl;
                    break;
                }
            } else {
                // message to any client, that server will chose
                string s = MESSAGE_FROM_USER + string("any,")
                    + to_string(getpid()) + string(",") 
                    + string("Hello from client ") + to_string(getpid()) + string("!!!");
                if (!SendMessage(serverQueueId, TMessageBuffer(s, getpid()))) {
                    cout << "Client (" << getpid() << ") has no access to servers queue. Client will be stopped" << endl;
                    break;
                }
            }
        }
        
        sleep(1);
    }

    SendInfoMessageToServer(serverQueueId, clientQueueId, false);
    DeleteQueue(clientQueueId);
    cout << "Client (" << getpid() << ") stopped" << endl;

    return 0;
}
