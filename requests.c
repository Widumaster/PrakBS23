//
// Created by Widumaster on 08.04.2023.
//

#include "requests.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include "threads.h"


#define BUFSIZE 1024

int count = 0;
char getreturn[2054];

void initCount()
{
    count = 0;
}

void requestAddClient(Server* server){

    int ClientID = accept(server->ServerID, 0, 0);
    server->ClientIDList[server->ClientIDListSize] = ClientID;
    server->ClientIDListSize++;
}

void requestPUT(pthreadInformation* pthreadExecute){
    initCount();
    while (pthreadExecute->server->store[count].myKey[0] != 0 && memcmp(pthreadExecute->server->store[count].myKey, pthreadExecute->commandStruct.commandKey,strlen(pthreadExecute->commandStruct.commandKey)) != 0) {
        count++;
        if (count >= 10) break;
    }
    if(pthreadExecute->server->store[count].lockData.flag != 1 || pthreadExecute->server->store[count].lockData.clientID == pthreadExecute->clientID)
    {

        openSem(pthreadExecute->server);
        strcpy(pthreadExecute->server->store[count].myKey, pthreadExecute->commandStruct.commandKey);
        strcpy(pthreadExecute->server->store[count].myValue, pthreadExecute->commandStruct.commandText);
        closeSem(pthreadExecute->server);

        //Muss noch in eine eigene Function gepackt werden um redundanz zu verhindern
        int index = 0;

        //PUB SUB Schleife
        while(pthreadExecute->server->store[count].PubSubDictionary[index] != 0 && index < 99){

            memset(getreturn, 0, 2054);
            int writtenByte = sprintf(getreturn, "Your Subscribed key changed\r\n");
            write(pthreadExecute->server->store[count].PubSubDictionary[index], getreturn, writtenByte);

            index++;
        }
    }else{
        int writtenByte = sprintf(getreturn, "Key locked\n");
        write(pthreadExecute->clientID, getreturn, writtenByte);
    }
}

void requestGET(pthreadInformation* pthreadExecute){
    initCount();
    while (pthreadExecute->server->store[count].myKey[0] != 0) {
        if (memcmp(pthreadExecute->server->store[count].myKey, pthreadExecute->commandStruct.commandKey,strlen(pthreadExecute->commandStruct.commandKey)) == 0) {
            if(pthreadExecute->server->store[count].lockData.flag != 1 || pthreadExecute->server->store[count].lockData.clientID == pthreadExecute->clientID)
            {
                memset(getreturn, 0, 2054);

                openSem(pthreadExecute->server);
                int writtenByte = sprintf(getreturn, "GET:%s:%s\n", pthreadExecute->server->store[count].myKey, pthreadExecute->server->store[count].myValue);
                closeSem(pthreadExecute->server);

                write(pthreadExecute->clientID, getreturn, writtenByte);
            }else{
                int writtenByte = sprintf(getreturn, "Key locked\n");
                write(pthreadExecute->clientID, getreturn, writtenByte);
            }
        }
        count++;
        if (count == 10) break;
    }
}

void requestDEL(pthreadInformation* pthreadExecute){

    initCount();
    unsigned char flag = 0;
    openSem(pthreadExecute->server);
    while (count < 8) {
        if (memcmp(pthreadExecute->server->store[count].myKey, pthreadExecute->commandStruct.commandKey,
                   strlen(pthreadExecute->commandStruct.commandKey)) == 0) {
            memset(pthreadExecute->server->store[count].myKey, 0, BUFSIZE);
            memset(pthreadExecute->server->store[count].myValue, 0, BUFSIZE);
            flag = 1;
        }

        if (flag) {
            strcpy(pthreadExecute->server->store[count].myKey, pthreadExecute->server->store[count + 1].myKey);
            strcpy(pthreadExecute->server->store[count].myValue, pthreadExecute->server->store[count + 1].myValue);

            memset(pthreadExecute->server->store[count + 1].myKey, 0, BUFSIZE);
            memset(pthreadExecute->server->store[count + 1].myValue, 0, BUFSIZE);
        }
        count++;
    }
    closeSem(pthreadExecute->server);
}

void requestBEG(pthreadInformation* pthreadExecute) {
    initCount();
    while (pthreadExecute->server->store[count].myKey[0] != 0) {
        if (memcmp(pthreadExecute->server->store[count].myKey, pthreadExecute->commandStruct.commandKey,strlen(pthreadExecute->commandStruct.commandKey)) == 0) {
            pthreadExecute->server->store[count].lockData.flag = 1;
            pthreadExecute->server->store[count].lockData.clientID = pthreadExecute->clientID;
        }
        count++;
        if (count >= 10) break;
    }
}

void requestEND(pthreadInformation* pthreadExecute){
    initCount();
    while (pthreadExecute->server->store[count].myKey[0] != 0) {
        if (memcmp(pthreadExecute->server->store[count].myKey, pthreadExecute->commandStruct.commandKey,strlen(pthreadExecute->commandStruct.commandKey)) == 0) {
            pthreadExecute->server->store[count].lockData.flag = 0;
            pthreadExecute->server->store[count].lockData.clientID = 0;
        }
        count++;
        if (count >= 10) break;
    }
}

void requestSUB(pthreadInformation* pthreadExecute) {


    //Muss später noch hinzufügen oder ändern das ich nicht immer durch alle Values durch iterieren möchte
    //beim disconnecten eines CLients, dewegen Dictionary vom KeyValue Store zu Client side wechseln
    //Implementiere ich später wenn ich mehr Aufgaben fertig habe und beim aufräumen meines codes bin

    initCount();
    while (pthreadExecute->server->store[count].myKey[0] != 0) {
        if (memcmp(pthreadExecute->server->store[count].myKey, pthreadExecute->commandStruct.commandKey,strlen(pthreadExecute->commandStruct.commandKey)) == 0) {

            int index = 0;
            while (pthreadExecute->server->store[count].PubSubDictionary[index] != 0)
            {
                index++;
            }
            pthreadExecute->server->store[count].PubSubDictionary[index] = pthreadExecute->clientID;

        }
        count++;
        if (count >= 10) break;
    }
}

void requestQUIT(pthreadInformation* pthreadExecute)
{
    unsigned char flag = 0;
    for (int i = 0; i < pthreadExecute->server->ClientIDListSize; ++i) {
        if(pthreadExecute->server->ClientIDList[i] == pthreadExecute->clientID){
            pthreadExecute->server->ClientIDList[i] = 0;
            pthreadExecute->server->ClientIDListSize--;
            flag = 1;
        }

        if(flag)
        {
            pthreadExecute->server->ClientIDList[i] = pthreadExecute->server->ClientIDList[i+1];
            pthreadExecute->server->ClientIDList[i+1] = 0;
        }
    }

    close(pthreadExecute->clientID);
}
