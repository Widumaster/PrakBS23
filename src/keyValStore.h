//
// Created by muragara on 4/3/23.
//

#ifndef PRAKBS23_KEYVALSTORE_H
#define PRAKBS23_KEYVALSTORE_H
#include "common.h"

int put(Message *arr, Message message);
int get(Message *arr, char* key, char* res);
int del(Message *arr, char* key);

#endif //PRAKBS23_KEYVALSTORE_H