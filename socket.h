//
// Created by Widumaster on 07.04.2023.
//

#ifndef PRAKBS21_SOCKET_H
#define PRAKBS21_SOCKET_H

typedef struct LockData_{
    unsigned char flag;
    int clientID;
}LockData;

typedef struct DataEntry_{
    char myKey[1024];
    char myValue[1024];
    LockData lockData;
    int PubSubDictionary[100];
}DataEntry;

typedef struct Server_
{
    int ServerID;
    int ClientIDList[100];
    int ClientIDListSize;
    DataEntry store[10];
    int SemaphoreID;
}Server;

typedef struct SocketOptions_{
    int socketDomain;
    int socketType;
    int socketProtocol;

    int socketLevel;
    int socketOptionname;
    const void* socketOptionValue;
    int socketOptionlength;

}SocketOptions;

SocketOptions createSocketOptions();   //For default socketOptions
Server GetSocket();
void createSocket(Server* server, SocketOptions* socketOptions);
void initServer(Server* server);
void setSocketOptions(Server* server, SocketOptions* socketOptions);
void bindSocket(Server* server);
void socketListen(Server* server);

#endif //PRAKBS21_SOCKET_H
