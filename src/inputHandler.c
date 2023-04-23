//
// Created by muragara on 4/13/23.
//

#include <string.h>
#include <stdio.h>

#include "inputHandler.h"
#include "keyValStore.h"

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

int parseInput(char cmd[CMDSIZE], char key[KEYSIZE], char value[VALSIZE], char* in){
    char tempString[BUFSIZE];
    char* token;

    memcpy(tempString, in, BUFSIZE);

    token = strtok(tempString, " ");
    if(token != NULL){
        strcpy(cmd, token);
    }

    if(strcmp(cmd, "GET") != 0 && strcmp(cmd, "DEL") != 0){
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

}

int handleInput(Message *arr, char* in){
    char cmd[CMDSIZE] = "";
    char key[KEYSIZE] = "";
    char value[VALSIZE] = "";

    parseInput(cmd, key, value, in);

    if(strncmp(cmd, "QUIT", 4) == 0){
        snprintf(in, BUFSIZE, "QUIT\n");
        return 1;
    }

    if(strcmp(cmd, "GET") == 0){
        handleGET(arr, key, in);
    }
    else if(strcmp(cmd, "PUT") == 0){
        handlePUT(arr, key, value, in);
    }
    else if(strcmp(cmd, "DEL") == 0){
        handleDEL(arr, key, in);
    }
    return 0;

   // strncpy(cmd, in, ptr);
}
