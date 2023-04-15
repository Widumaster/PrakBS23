//
// Created by muragara on 4/4/23.
//
#include <stdio.h>
#include <stdlib.h>
#include "dynArray.h"


void initArray(Array *arr, size_t initSize){
    arr->array = malloc(initSize * sizeof(Message));
    arr->used = 0;
    arr->size = initSize;
}

void insertArray(Array *arr, Message message){
    if(arr->used == arr->size){
        arr->size *= 2;
        arr->array = realloc(arr->array, arr->size * sizeof(Message));
    }
    arr->array[arr->used++] = message;
}

void freeArray(Array* arr){
    for (int i = 0; i < arr->used; ++i) {
        free(arr->array[i].key);
        free(arr->array[i].value);
    }

    free(arr->array);
    arr->array = NULL;
    arr->used = 0;
    arr->size = 0;
}

void printArray(Array* arr){
    for(int i = 0; i < arr->used; i++){
        printf("Key: %s, Value: %s, Deleted: %d \n", arr->array[i].key, arr->array[i].value, arr->array[i].deleted);
    }
}