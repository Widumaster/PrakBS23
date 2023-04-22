//
// Created by Widumaster on 08.04.2023.
//
#include "socket.h"
#include "commandStruct.h"

#ifndef PRAKBS21_REQUESTS_H
#define PRAKBS21_REQUESTS_H

void requestAddClient(Server* server);
void requestPUT(Server* server, CommandStruct commandStruct, int clientID);
void requestGET(Server* server, CommandStruct commandStruct, int clientID);
void requestDEL(Server* server, CommandStruct commandStruct);
void requestBEG(Server* server, CommandStruct commandStruct, int clientID);
void requestEND(Server* server, CommandStruct commandStruct);
void requestSUB(Server* server, CommandStruct commandStruct, int clientID);
void requestQUIT(Server* server, int clientID);

#endif //PRAKBS21_REQUESTS_H

// All Requests
/*
    CommandPUT
    CommandGET
    CommandDEL
    CommandBEG
    CommandEND
    CommandSUB
    CommandQUIT
  */