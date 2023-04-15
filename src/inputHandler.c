//
// Created by muragara on 4/13/23.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "inputHandler.h"
#include "keyValStore.h"

#define BUFSIZE 1024
#define VALSIZE 1000
#define KEYSIZE 20
#define CMDSIZE 4

void handleGET(Array *arr, char* key, char* res){
    char value[VALSIZE] = "";

    if(get(arr, key, value)){
        strcpy(res, "KEY DOES NOT EXIST\n");
    }else{
        snprintf(res, BUFSIZE, "GET:%s:%s\n", key, value);
    }
}

void handlePUT(Array *arr, char* key, char* value, char* res){
    Message message = {
            .key = malloc(strlen(key) + 1),
            .value = malloc(strlen(value) + 1),
            .deleted = 0,
    };
    strcpy(message.key, key);
    strcpy(message.value, value);

    put(arr, message);

    snprintf(res, BUFSIZE, "PUT:%s:%s\n", key, value);
}

void handleDEL(Array *arr, char* key, char* in){
    return;
}

int handleInput(Array *arr, char* in){
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
        //handleDEL();
    }

   // strncpy(cmd, in, ptr);
}