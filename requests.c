//
// Created by Widumaster on 08.04.2023.
//

#include "requests.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>


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

void requestPUT(Server* server, CommandStruct commandStruct, int clientID){
    initCount();
    while (server->store[count].myKey[0] != 0 && memcmp(server->store[count].myKey, commandStruct.commandKey,strlen(commandStruct.commandKey)) != 0) {
        count++;
        if (count >= 10) break;
    }
    if(server->store[count].lockData.flag != 1 || server->store[count].lockData.clientID == clientID)
    {
        strcpy(server->store[count].myKey, commandStruct.commandKey);
        strcpy(server->store[count].myValue, commandStruct.commandText);

        /*if(memcmp(server->store[count].myKey, server->Dictionary.keyList,strlen(server->store[count].myKey))==0)
        {
            int writtenByte = sprintf(getreturn, "Your Subscribed key changed\n");
            write(server->Dictionary.clientIDList, getreturn, writtenByte);
        }*/

    }else{
        int writtenByte = sprintf(getreturn, "Key locked\n");
        write(clientID, getreturn, writtenByte);
    }
}

void requestGET(Server* server, CommandStruct commandStruct, int clientID){
    initCount();
    while (server->store[count].myKey[0] != 0) {
        if (memcmp(server->store[count].myKey, commandStruct.commandKey,strlen(commandStruct.commandKey)) == 0) {
            if(server->store[count].lockData.flag != 1 || server->store[count].lockData.clientID == clientID)
            {
                memset(getreturn, 0, 2054);
                int writtenByte = sprintf(getreturn, "GET:%s:%s\n", server->store[count].myKey, server->store[count].myValue);
                write(clientID, getreturn, writtenByte);
            }else{
                int writtenByte = sprintf(getreturn, "Key locked\n");
                write(clientID, getreturn, writtenByte);
            }
        }
        count++;
        if (count == 10) break;
    }
}

void requestDEL(Server* server, CommandStruct commandStruct){

    initCount();
    unsigned char flag = 0;
    while (count < 8) {
        if (memcmp(server->store[count].myKey, commandStruct.commandKey,
                   strlen(commandStruct.commandKey)) == 0) {
            memset(server->store[count].myKey, 0, BUFSIZE);
            memset(server->store[count].myValue, 0, BUFSIZE);
            flag = 1;
        }

        if (flag) {
            strcpy(server->store[count].myKey, server->store[count + 1].myKey);
            strcpy(server->store[count].myValue, server->store[count + 1].myValue);

            memset(server->store[count + 1].myKey, 0, BUFSIZE);
            memset(server->store[count + 1].myValue, 0, BUFSIZE);
        }
        count++;
    }
}

void requestBEG(Server* server, CommandStruct commandStruct, int clientID) {
    initCount();
    while (server->store[count].myKey[0] != 0) {
        if (memcmp(server->store[count].myKey, commandStruct.commandKey,strlen(commandStruct.commandKey)) == 0) {
            server->store[count].lockData.flag = 1;
            server->store[count].lockData.clientID = clientID;
        }
        count++;
        if (count >= 10) break;
    }
}

void requestEND(Server* server, CommandStruct commandStruct) {
    initCount();
    while (server->store[count].myKey[0] != 0) {
        if (memcmp(server->store[count].myKey, commandStruct.commandKey,strlen(commandStruct.commandKey)) == 0) {
            server->store[count].lockData.flag = 0;
            server->store[count].lockData.clientID = 0;
        }
        count++;
        if (count >= 10) break;
    }
}

void requestSUB(Server* server, CommandStruct commandStruct, int clientID) {
    initCount();
    while (server->store[count].myKey[0] != 0) {
        if (memcmp(server->store[count].myKey, commandStruct.commandKey,strlen(commandStruct.commandKey)) == 0) {

            int index = 0;
            while (server->store[count].PubSubDictionary[index] != 0)
            {
                index++;
            }
            server->store[count].PubSubDictionary[index] = clientID;

        }
        count++;
        if (count >= 10) break;
    }
}

void requestQUIT(Server* server, int clientID)
{
    unsigned char flag = 0;
    for (int i = 0; i < server->ClientIDListSize; ++i) {
        if(server->ClientIDList[i] == clientID){
            server->ClientIDList[i] = 0;
            server->ClientIDListSize--;
            flag = 1;
        }

        if(flag)
        {
            server->ClientIDList[i] = server->ClientIDList[i+1];
            server->ClientIDList[i+1] = 0;
        }
    }

    close(clientID);
}
