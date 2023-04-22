//
// Created by Widumaster on 07.04.2023.
//
#define PORT 5678

#include "socket.h"
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

Server GetSocket(){

    Server server;
    SocketOptions socketOptions = createSocketOptions();

    createSocket(&server, &socketOptions);
    setSocketOptions(&server, &socketOptions);
    bindSocket(&server);
    socketListen(&server);

    initServer(&server);
    return server;
}

void initServer(Server* server)
{
    server->ClientIDList[server->ClientIDListSize] = server->ServerID;
    server->ClientIDListSize++;
}

void createSocket(Server* server, SocketOptions* socketOptions){

    // Socket erstellen
    server->ServerID = socket(socketOptions->socketDomain, socketOptions->socketType, socketOptions->socketProtocol);
    //socketOptions.socketFileDescriptor = server->ServerID;
    if (server->ServerID < 0 ){
        fprintf(stderr, "socket konnte nicht erstellt werden\n");
        exit(-1);
    }
}

void setSocketOptions(Server* server, SocketOptions* socketOptions){
    // Socket Optionen setzen für schnelles wiederholtes Binden der Adresse
    int option = 1;
    setsockopt(server->ServerID, socketOptions->socketLevel, socketOptions->socketOptionname, socketOptions->socketOptionValue, socketOptions->socketOptionlength);
}

void bindSocket(Server* server){
    // Socket binden
    struct sockaddr_in serverConnectionInfo;
    serverConnectionInfo.sin_family = AF_INET;
    serverConnectionInfo.sin_addr.s_addr = INADDR_ANY;
    serverConnectionInfo.sin_port = htons(PORT);
    int brt = bind(server->ServerID, (struct sockaddr *) &serverConnectionInfo, sizeof(serverConnectionInfo));
    if (brt < 0 ){
        fprintf(stderr, "socket konnte nicht gebunden werden\n");
        exit(-1);
    }
}

void socketListen(Server* server){
    // Socket lauschen lassen
    int lrt = listen(server->ServerID, 5);
    if (lrt < 0 ){
        fprintf(stderr, "socket konnte nicht listen gesetzt werden\n");
        exit(-1);
    }
}

SocketOptions createSocketOptions(){
    SocketOptions socketOptions;
    int option = 1;

    socketOptions.socketDomain = AF_INET;
    socketOptions.socketType = SOCK_STREAM;
    socketOptions.socketProtocol = 0;

    socketOptions.socketLevel = SOL_SOCKET;
    socketOptions.socketOptionname = SO_REUSEADDR;
    socketOptions.socketOptionValue = (const void *) &option;
    socketOptions.socketOptionlength = sizeof(int);

    return socketOptions;
}
