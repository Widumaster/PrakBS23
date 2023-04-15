//
// Created by muragara on 4/3/23.
//

#ifndef PRAKBS23_KEYVALSTORE_H
#define PRAKBS23_KEYVALSTORE_H
#include "common.h"

int put(Array *arr, Message message);
int get(Array *arr, char* key, char* res);
int del(Array *arr, char* key);

#endif //PRAKBS23_KEYVALSTORE_H