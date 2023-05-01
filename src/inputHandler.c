//
// Created by muragara on 4/13/23.
//

#include <string.h>
#include <stdio.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>

#include "inputHandler.h"
#include "keyValStore.h"
#include "cmdEnum.h"


#define VALSIZE 2000
//#define KEYSIZE 43
#define CMDSIZE 5

void handleGET(Message *arr, char* key, char* res){
    char value[VALSIZE] = "";

    if(get(arr, key, value) == 1){
        snprintf(res, BUFSIZE, "GET:%s:key_nonexistent\n", key);
    }else{
        snprintf(res, BUFSIZE, "GET:%s:%s\n", key, value);
    }
}

void handlePUT(Message *arr, char* key, char* value, char* res, int qid){
    Message message = {
            .key = "",
            .value = ""
    };

    strcpy(message.key, key);
    strcpy(message.value, value);

    if(put(arr, message) == 1){
        strcpy(res, "PUT FAILED: NO EMPTY SPACE");
    } else{
        snprintf(res, BUFSIZE, "PUT:%s:%s\n", key, value);
        SubMessage subMessage = {
                .mtype = 1,
                .value = "",
                .key = "",
                .cmd = PUT
        };
        strcpy(subMessage.key, key);
        strcpy(subMessage.value, value);

        // send message to queue with msgsnd
        int t = msgsnd(qid, &subMessage, sizeof(SubMessage), 0);
        if ( t == -1) {
            perror("msgsnd");
            exit(1);
        }
    }
}

void handleDEL(Message *arr, char* key, char* res){
    if(del(arr, key) == 1){
        snprintf(res, BUFSIZE, "GET:%s:key_nonexistent\n", key);
    } else{
        snprintf(res, BUFSIZE, "DEL:%s:key_deleted\n", key);
    }
}

void handleSUB(Message *messageArr, SubscriberStore *subArr, char* key, char* res, int clientQueue, int pid){
    // add subscriber to subscriber array
    if(putSubscriber(subArr, key, pid, clientQueue) == 1){
        strcpy(res, "SUB FAILED: ALREADY SUBBED OR NO EMPTY SPACE\n");
    } else{
        printf("%i subbed to %s\n", getpid(), key);
        char value[VALSIZE] = "";
        get(messageArr, key, value);
        snprintf(res, BUFSIZE, "SUB:%s:%s\n", key, value);
    }
}

enum CMD parseInput(char key[KEYSIZE], char value[VALSIZE], char* in){
    char tempString[BUFSIZE];
    char* token;
    char cmdString[CMDSIZE] = "";
    enum CMD cmdResult;

    memcpy(tempString, in, BUFSIZE);

    token = strtok(tempString, " ");
    if(token != NULL){
        strcpy(cmdString, token);
    }
    printf("CMD: %s\n", cmdString);

    cmdResult = getCmdValue(cmdString);
    printf("CMD: %s\n", getCmdString(cmdResult));

    if(cmdResult != GET && cmdResult != DEL && cmdResult != SUB){
        token = strtok(NULL, " ");
        if(token != NULL){
            strcpy(key, token);
        }

        token = strtok(NULL, "\r\n");
        if(token != NULL){
            strcpy(value, token);
        }
    }
    else{
        token = strtok(NULL, "\r\n");
        if(token != NULL){
            strcpy(key, token);
        }
    }
    return cmdResult;
}

int handleInput(Message *messageArr, SubscriberStore *subArr, char* in, int qid, int clientQueue){
    enum CMD cmd;
    char key[KEYSIZE] = "";
    char value[VALSIZE] = "";

    cmd = parseInput(key, value, in);
    printf("CMD: %s: %s: %s\n", getCmdString(cmd), key, value);

    switch (cmd) {
        case GET: handleGET(messageArr, key, in); break;
        case PUT: handlePUT(messageArr, key, value, in, qid); break;
        case DEL: handleDEL(messageArr, key, in); break;
        case QUIT:
        case BEG:
        case END: /*snprintf(in, BUFSIZE, "%s\n", getCmdString(cmd)); */memset(in, 0, BUFSIZE) ;break;
        case SUB: handleSUB(messageArr, subArr, key, in, clientQueue, getpid()); break;
        default: cmd = ERR;
    }
    return cmd;

   // strncpy(cmd, in, ptr);
}
