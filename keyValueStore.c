//
// Created by Widumaster on 07.04.2023.
//
#define BUFSIZE 1024 // Größe des Buffers


#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/poll.h>
#include "keyValueStore.h"
#include "requests.h"
//#include "commandStruct.h" ? why does it work without it ????


void pollManager(Server* server)
{
    const int socketDataListSize = server->ClientIDListSize;
    struct pollfd* socketDataList[100];
    int timeout = -1;
    unsigned char flag = 0;


    memset(socketDataList, 0, sizeof(socketDataList));
    for (int i = 0; i < socketDataListSize; ++i)
    {
        struct pollfd* socketDataListCurrent = &socketDataList[i];

        socketDataListCurrent->fd = server->ClientIDList[i];
        socketDataListCurrent->events = POLLRDNORM;
        socketDataListCurrent->revents = 0;
    }


    const int amount = poll(socketDataList, socketDataListSize, timeout); // Not thread safe??
    const char success = amount != -1;

    //Beim iterieren merken wo ich was rausgelesen habe damit ich nicht unnötig immer von vorne starte
    //Weniger performance bei mehr Clients
    for (int i = 0; i < amount; i++)
    {
        for (int j = 0; j < 99; ++j)
        {
            const struct pollfd* const currentPollData = &socketDataList[j];

            if(currentPollData->revents != 0)
            {
                switch (currentPollData->revents)
                {
                    case POLLERR: // An error has occurred.
                    case POLLHUP: // A stream - oriented connection was either disconnected or aborted.
                    {
                        //PXSocketEventReadUnregister(pxSocket, currentPollData->fd);
                        break;
                    }
                    case POLLNVAL: // An invalid socket was used.
                    case POLLPRI: // Priority data may be read without blocking.This flag is not returned by the Microsoft Winsock provider.
                    case POLLRDBAND: // Priority band(out - of - band) data may be read without blocking.
                    case POLLRDNORM: // Normal data may be read without blocking.
                    {
                        //PXSocketReadPendingHandle(pxSocket, currentPollData->fd);
                        // check if same Client, if same client new client inc. if not same read command
                        char isNewClient = server->ServerID == currentPollData->fd;
                        if(isNewClient)
                        {
                            requestAddClient(server);
                        }else{
                            //thread
                            handleRequest(currentPollData->fd, server);
                        }
                        flag = 1;
                        break;
                    }
                    case POLLWRNORM: // Normal data may be written without blocking.

                    default:
                        break;
                }
            }
            if(flag) break;
        }
        flag = 0;
    }
}

int handleRequest(int clientID, Server* server) {

    char input[BUFSIZE];
    int bytes_read;
    char getreturn[2054];
    unsigned char flag = 0;

    // Lesen von Daten, die der Client schickt
    memset(input, 0, BUFSIZE);
    bytes_read = read(clientID, input, BUFSIZE);

    CommandStruct commandStruct;

    // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
    //while (bytes_read > 0) {
    for (int i = 0; i < bytes_read; ++i) {
        char flag2 = input[i] == '\n' || input[i] == '\r';
        if (flag2) {
            input[i] = '\0';
            bytes_read--;
        }
    }

    CommandStructInitialize(&commandStruct);
    CommandStructConstruct(&commandStruct, input);

    printf("Ausgelesener Key: %s\n", commandStruct.commandKey);
    printf("Ausgelesener CommandText: %s\n", commandStruct.commandText);

    int count = 0;
    switch (commandStruct.commandType) {
        case CommandPUT:
            requestPUT(server, commandStruct, clientID);
            break;

        case CommandGET:
            requestGET(server, commandStruct, clientID);
            break;

        case CommandDEL:
            requestDEL(server, commandStruct);
            break;

        case CommandBEG:
            requestBEG(server, commandStruct, clientID);
            break;

        case CommandEND:
            requestEND(server, commandStruct);
            break;

        case CommandSUB:
            requestSUB(server, commandStruct, clientID);
            break;

        case CommandQUIT:
            requestQUIT(server, clientID);
    }

    //printf("sending back the %d bytes I received...\n", bytes_read);
    //input[bytes_read++ - 1] = '\r';
    //input[bytes_read++ - 1] = '\n';
    //input[bytes_read - 1] = '\0';
    //write(clientID, input, bytes_read);
    //memset(input, 0, BUFSIZE);
    //bytes_read = read(clientID, input, BUFSIZE);
    //close(clientID);
    //}
}