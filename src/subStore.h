//
// Created by muragara on 4/27/23.
//

#ifndef PRAKBS23_SUBSTORE_H
#define PRAKBS23_SUBSTORE_H

#include "common.h"

#define KEYSIZE 43
#define MAXKEYS 512
#define MAXSUBS 1024

typedef struct {
    int pid;
    int clientQueue;
} Subscriber;

typedef struct {
    long mtype;
    char mtext[BUFSIZE];
    int keyIndex;

} PubMessage;

// KEY + PID
typedef struct {
    char key[KEYSIZE];
    Subscriber subs[MAXSUBS];
} SubscriberStore;

typedef struct {
    long mtype;
    char value[2000];
    char key[KEYSIZE];
    int cmd;
} SubMessage;

int putSubscriber(SubscriberStore *arr, char key[KEYSIZE], int pid, int clientQueue);
int getSubscriber(SubscriberStore *arr, char* key, int* res);
int delSubscriber(SubscriberStore *arr, char key[KEYSIZE], int pid);

#endif //PRAKBS23_SUBSTORE_H
