//
// Created by muragara on 4/13/23.
//

#include <string.h>
#include <stdio.h>

#include "inputHandler.h"
#include "keyValStore.h"

#define VALSIZE 2000
#define KEYSIZE 44
#define CMDSIZE 4

void handleGET(Message *arr, char* key, char* res){
    char value[VALSIZE] = "";

    if(get(arr, key, value) == 1){
        strcpy(res, "KEY DOES NOT EXIST\n");
    }else{
        snprintf(res, BUFSIZE, "GET:%s:%s\n", key, value);
    }
}

void handlePUT(Message *arr, char* key, char* value, char* res){
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
        strcpy(res, "DEL FAILED");
    } else{
        snprintf(res, BUFSIZE, "DEL:%s\n", key);
    }
}

int handleInput(Message *arr, char* in){
    char tempString[BUFSIZE];
    char cmd[CMDSIZE] = "";
    char key[KEYSIZE] = "";
    char value[VALSIZE] = "";
    char* token;

    memcpy(tempString, in, BUFSIZE);

    token = strtok(tempString, " ");
    if(token != NULL){
        strcpy(cmd, token);
    }

    if(strcmp(cmd, "GET") != 0){
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


    if(strcmp(cmd, "GET") == 0){
        handleGET(arr, key, in);
    }
    else if(strcmp(cmd, "PUT") == 0){
        handlePUT(arr, key, value, in);
    }
    else if(strcmp(cmd, "DEL") == 0){
        handleDEL(arr, key, in);
    }

   // strncpy(cmd, in, ptr);
}
