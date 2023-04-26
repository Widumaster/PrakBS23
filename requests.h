//
// Created by Widumaster on 08.04.2023.
//
#ifndef PRAKBS21_REQUESTS_H
#define PRAKBS21_REQUESTS_H

#include "socket.h"
#include "commandStruct.h"
#include "threads.h"

void requestAddClient(Server* server);
void requestPUT(pthreadInformation* pthreadExecute);
void requestGET(pthreadInformation* pthreadExecute);
void requestDEL(pthreadInformation* pthreadExecute);
void requestBEG(pthreadInformation* pthreadExecute);
void requestEND(pthreadInformation* pthreadExecute);
void requestSUB(pthreadInformation* pthreadExecute);
void requestQUIT(pthreadInformation* pthreadExecute);

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


/* OLD function header
void requestPUT(Server* server, CommandStruct commandStruct, int clientID);
void requestGET(Server* server, CommandStruct commandStruct, int clientID);
void requestDEL(Server* server, CommandStruct commandStruct);
void requestBEG(Server* server, CommandStruct commandStruct, int clientID);
void requestEND(Server* server, CommandStruct commandStruct);
void requestSUB(Server* server, CommandStruct commandStruct, int clientID);
void requestQUIT(Server* server, int clientID);*/