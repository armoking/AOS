#include "common.h"

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
    if (response < 0) {
        perror("Error while gettin queue state");
    }
    return ds;
}

const string MESSAGE_FROM_USER = "user-message:";
const string INFO_MESSAGE = "user-info:";

enum MESSAGE_TYPES {
    INFO_MESSAGE_TYPE = 1,
    USER_MESSAGE_TYPE = 2
};

bool StartsWith(string s, string t) {
    return s.size() >= t.size() && s.substr(0, t.size()) == t;
}

tuple<int, int, int> ParseInfoMessage(string s) {
    if (!StartsWith(s, INFO_MESSAGE)) {
        return {-1, -1, -1};
    }
    int i = INFO_MESSAGE.size();
    vector<string> data(3);
    int c = 0;
    while (i < s.size()) {
        char ch = s[i];
        if (ch == ',') {
            c++;
        } else {
            data[min(c, 2)].push_back(ch);
        }
        i++;
    }
    if (c != 2) {
        return {-1, -1, -1};
    }
    int pid = 0;
    int qid = 0;
    int status = 0;
    try {
        pid = stoi(data[0]);
        qid = stoi(data[1]);
        status = stoi(data[2]);
        return {pid, qid, status};
    } catch (...) {
        return {-1, -1, -1};
    }
}

tuple<int, int, string> ParseUserMessage(string s) {
    int prefSize = MESSAGE_FROM_USER.size();
    if (!StartsWith(s, MESSAGE_FROM_USER) || prefSize + 4 > s.size() || s[prefSize + 3] != ',') {
        return {-1, -1, ""};
    }
    string forWhom = s.substr(prefSize, 3);
    int t = 0;
    if (forWhom == "any") {
        t = 1;
    } else if (forWhom == "all") {
        t = 0;
    } else {
        return {-1, -1, ""};
    }
    int pos = prefSize += 4;
    s = s.substr(prefSize, s.size() - prefSize);
    pos = s.find(',');
    if (pos == string::npos || pos == 0) {
        return {-1, -1, ""};
    }
    
    string idS = s.substr(0, pos);
    string txt = s.substr(pos + 1, s.size() - pos - 1);
    try {
        int pid = stoi(idS);
        return {t, pid, txt};
    } catch (...) {
        return {-1, -1, ""};
    }
}


int serverQueueId;

void action(int sig) {
    activeProcesses--;
    if (activeProcesses == 0) {
        DeleteQueue(serverQueueId);
    }

    cout << "Server (" << getpid() << ") stopped" << endl;
    exit(0);
}

int GetServerQueueId() {
    key_t key = ftok("armoking_server_queue", 123); // server queue id
    serverQueueId = msgget(key, 0660);
    if (serverQueueId == -1) {
        serverQueueId = msgget(key, IPC_CREAT | 0660);
    }
    activeProcesses = 1;
    return serverQueueId;
 }


// server - side
int main(int argc, char* argv[]) {
    signal(SIGINT, action);
     
    int serverQueueId = GetServerQueueId();
    map<int, int> clients;

    while (true) {
        cout << "Server queue id " << serverQueueId << ". ps = " << activeProcesses << endl;
        int serverQueueSize = GetAndLogQueueState(serverQueueId).msg_qnum;
        if (serverQueueSize != 0) {
            for (int i = 0; i < serverQueueSize; i++) {
                TMessageBuffer msg = RecieveMessage(serverQueueId, -2); // like std::set
                cout << "Server (" << getpid() << ") recieved message with type = " << msg.type << ". Message = " << msg.buffer << endl;
                string s(msg.buffer);
                if (StartsWith(s, INFO_MESSAGE)) {
                    auto [clientId, clientQueueId, clientStatus] = ParseInfoMessage(s);
                    if (clientId < 0) {
                        cout << "Failed to parse info message. Message: " << s << endl;
                    } else {
                        cout << "Client (" << clientId << ") with queueId (" << clientQueueId << ") " << (clientStatus ? "started" : "stopped") << endl;
                        if (clientStatus) {
                            clients[clientId] = clientQueueId;
                        } else {
                            clients.erase(clientId);
                        }
                    }
                } else if (StartsWith(s, MESSAGE_FROM_USER)) {
                    cout << "Server recieved message from client: " << s << endl;
                    auto [forWhom, fromId, txt] = ParseUserMessage(s);    
                    if (forWhom == -1) {
                        cout << "Parsing message fail. message = " << s << endl;
                    } else {
                        cout << "Client (" << fromId << ") send message with: " << forWhom << ' ' << txt << endl;
                        if (forWhom) {
                            // for any
                            int tot = clients.size();
                            int pos = rand() % tot;
                            int ind = 0;
                            for (auto [clientId, clientQueueId] : clients) {
                                if (ind == pos) {
                                    // send message to this client
                                    string prefix = string("[Server proxy message from client (") + to_string(fromId) + string(")]:"); 
                                    SendMessage(clientQueueId, TMessageBuffer(prefix + txt, clientId));
                                }
                                ind++;
                            }
                        } else {
                            // for all
                            for (auto [clientId, clientQueueId] : clients) {
                                // send message to this client
                                string prefix = string("[Server proxy message from client (") + to_string(fromId) + string(")]:"); 
                                SendMessage(clientQueueId, TMessageBuffer(prefix + txt, clientId));
                            }
                        }
                    }
                } else {
                    cout << "Server cant parse message: " << s << endl;
                } 
            }
        }
        sleep(1);
    }


    action(0);
}
