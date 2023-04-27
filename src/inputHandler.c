//
// Created by muragara on 4/13/23.
//

#include <string.h>
#include <stdio.h>

#include "inputHandler.h"
#include "keyValStore.h"
#include "cmdEnum.h"

#define VALSIZE 2000
#define KEYSIZE 43
#define CMDSIZE 5

void handleGET(Message *arr, char* key, char* res){
    char value[VALSIZE] = "";

    if(get(arr, key, value) == 1){
        snprintf(res, BUFSIZE, "GET:%s:key_nonexistent\n", key);
    }else{
        snprintf(res, BUFSIZE, "GET:%s:%s\n", key, value);
    }
}

int handlePUT(Message *arr, char* key, char* value, char* res){
    Message message = {
            .key = "",
            .value = "",
            .deleted = 0,
    };

    strcpy(message.key, key);
    strcpy(message.value, value);

    if(put(arr, message) == 1){
        strcpy(res, "PUT FAILED: NO EMPTY SPACE");
    } else{
        snprintf(res, BUFSIZE, "PUT:%s:%s\n", key, value);
    }


}

void handleDEL(Message *arr, char* key, char* res){
    if(del(arr, key) == 1){
        snprintf(res, BUFSIZE, "GET:%s:key_nonexistent\n", key);
    } else{
        snprintf(res, BUFSIZE, "DEL:%s:key_deleted\n", key);
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

    if(cmdResult != GET && cmdResult != DEL){
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

int handleInput(Message *arr, char* in){
    enum CMD cmd;
    char key[KEYSIZE] = "";
    char value[VALSIZE] = "";

    cmd = parseInput(key, value, in);
    printf("CMD: %s: %s: %s\n", getCmdString(cmd), key, value);

    switch (cmd) {
        case GET: handleGET(arr, key, in); break;
        case PUT: handlePUT(arr, key, value, in); break;
        case DEL: handleDEL(arr, key, in); break;
        case QUIT:
        case BEG:
        case END: snprintf(in, BUFSIZE, "%s\n", getCmdString(cmd)); break;
        default: cmd = ERR;
    }
    return cmd;

   // strncpy(cmd, in, ptr);
}
