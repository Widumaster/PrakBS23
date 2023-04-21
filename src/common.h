//
// Created by muragara on 4/4/23.
//

#ifndef PRAKBS23_COMMON_H
#define PRAKBS23_COMMON_H

typedef struct{
    volatile char key[20];
    volatile char value[1000];
    volatile u_int8_t deleted;
} Message;

typedef struct {
    volatile Message *array;
    volatile size_t used;
    volatile size_t size;
} Array;

#endif //PRAKBS23_COMMON_H
