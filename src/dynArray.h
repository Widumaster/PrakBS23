//
// Created by muragara on 4/4/23.
//

#ifndef PRAKBS23_DYNARRAY_H
#define PRAKBS23_DYNARRAY_H

#include "common.h"

void initArray(Array *arr, size_t initSize);
void insertArray(Array *arr, Message message);
void freeArray(Array* arr);
void printArray(Array* arr);


#endif //PRAKBS23_DYNARRAY_H