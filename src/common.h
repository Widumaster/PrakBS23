//
// Created by muragara on 4/4/23.
//

#ifndef PRAKBS23_COMMON_H
#define PRAKBS23_COMMON_H

typedef struct{
    char* key;
    char* value;
    int deleted;
} Message;

typedef struct {
    Message *array;
    size_t used;
    size_t size;
} Array;

#endif //PRAKBS23_COMMON_H
