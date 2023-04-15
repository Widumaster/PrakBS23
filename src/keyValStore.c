//
//
// Created by muragara on 4/3/23.
#include <stdio.h>
#include <string.h>
#include "keyValStore.h"
#include "dynArray.h"

int put(Array *arr, Message message){
    for(int i = 0; i < arr->used; i++){
        if(strcmp(arr->array[i].key, message.key) == 0){
            strcpy(arr->array[i].value, message.value);
            return 0;
        }
    }

    insertArray(arr,message);
    return 0;
}

int get(Array *arr, char* key, char* res){
    for (int i = 0; i < arr->used; i++) {
        if(strcmp(arr->array[i].key, key) == 0){
            strcpy(res, arr->array[i].value);
            return 0;
        }
    }
    return 1;
}

int del(Array *arr, char* key){
    for (int i = 0; i < arr->used; i++) {
        if(strcmp(arr->array[i].key, key) == 0){
            arr->array[i].deleted = 1;
            return 0;
        }
    }
    return 1;
}


int loadMessages(char* key, char* res){
    FILE *file;

    if((file = fopen ("./data", "r")) == NULL){
        printf("Error!");
        return 1;
    }

    char line[512];
    while (fgets(line, sizeof(line), file)){
        char* seperator = strchr(line, ',');
        if(line){
            return 0;
        }
    }

    return 0;
}
