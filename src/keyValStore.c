//
//
// Created by muragara on 4/3/23.
#include <string.h>
#include "keyValStore.h"

int put(Message *arr, Message message){
    for(int i = 0; i < ARRSIZE; i++){
        if(strcmp(arr[i].key, "") == 0){
            arr[i] = message;
            return 0;
        }

        if(strcmp(arr[i].key, message.key) == 0){
            strcpy(arr[i].value, message.value);
            arr[i].deleted = 0;
            return 0;
        }
    }

    return 1;
}

int get(Message *arr, char* key, char* res){
    for (int i = 0; i < ARRSIZE; i++) {
        if(strcmp(arr[i].key, key) == 0 && arr[i].deleted == 0){
            strcpy(res, arr->value);
            return 0;
        }
    }
    return 1;
}

int del(Message *arr, char* key){
    for (int i = 0; i < ARRSIZE; i++) {
        if(strcmp(arr[i].key, key) == 0){
            arr[i].deleted = 1;
            return 0;
        }
    }
    return 1;
}
