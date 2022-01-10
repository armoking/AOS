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
    1. Создать очередь сообщений или получить доступ к уже существующей очереди.
    Вывести на экран статистическую информацию об очереди сообщений. Поместить несколько сообщений различных типов в очередь.
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

TMessageBuffer RecieveMessage(int queueId, int messageType) {
    TMessageBuffer message;

    int response = msgrcv(queueId, reinterpret_cast<struct msgbuf*>(&message), MAX_BUFFER_SIZE, messageType, 0);
    if (response >= 0) {
        message.type = messageType;
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

int main(int argc, char* argv[]) { 
    int queueId = msgget((key_t)1234, IPC_CREAT | 0660);
    {
        TMessageBuffer message1("Hello there 1!", 1);
        TMessageBuffer message2("Hello there 2!", 2);
        TMessageBuffer message3("Hello there 3!", 3);
        
        SendMessage(queueId, message1);
        SendMessage(queueId, message2);

        GetAndLogQueueState(queueId);
        
        SendMessage(queueId, message3);
    }   

    GetAndLogQueueState(queueId);
    {
        TMessageBuffer message3 = RecieveMessage(queueId, 3);
        cout << message3.buffer << endl;
        GetAndLogQueueState(queueId);
        
        TMessageBuffer message2 = RecieveMessage(queueId, 2);
        cout << message2.buffer << endl;
        
        TMessageBuffer message1 = RecieveMessage(queueId, 1);
        cout << message1.buffer << endl;
        GetAndLogQueueState(queueId);
    }
    DeleteQueue(queueId);
}

