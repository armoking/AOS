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
        perror("Error while deleting queue");
    }
    return ds;
}

// client - side
int main(int argc, char* argv[]) {
    key_t key = ftok("armoking_server_queue", 123); // for server queue id
    int queueId = msgget(key, 0660);
    while (queueId < 0) {
        cout << "Client (" << getpid() << ") can't handle server queueId" << endl;
        sleep(1);
        queueId = msgget(key, 0660);  
    }

    cout << "Client with PID = " << getpid() << " started." << endl;
    int clientQueueId = msgget(getpid() + 342 /* random number */, 0660 | IPC_CREAT);
    {
        string messageToServer = to_string(getpid()) + string(",") + to_string(clientQueueId); 
        TMessageBuffer messageForServer(messageToServer, 12345);
        SendMessage(queueId, messageForServer);
    }

    cout << "Server: " << queueId << " client: " << clientQueueId << endl;

    while (true) {
        int len = GetAndLogQueueState(clientQueueId).msg_qnum;
        if (len != 0) {
            TMessageBuffer msg = RecieveMessage(clientQueueId, 0);
            cout << "Client recieved message from server: " << msg.buffer << endl; 
            break;
        } else {
            sleep(1);
        }
    }
    
    DeleteQueue(clientQueueId);
    cout << "Client (" << getpid() << ") has stoped." << endl;
    exit(0);
}
