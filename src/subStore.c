//
// Created by muragara on 4/27/23.
//

#include <string.h>
#include "subStore.h"

int putSubscriber(SubscriberStore *arr, char key[KEYSIZE], int pid, int clientQueue){
    for(int i = 0; i < MAXKEYS; i++){
        if(strcmp(arr[i].key, key) == 0){
            for (int j = 0; j < MAXSUBS; j++) {
                if (arr[i].subs[j].pid == pid)
                    return 1;
                if (arr[i].subs[j].pid == 0){
                    arr[i].subs[j].pid = pid;
                    arr[i].subs[j].clientQueue = clientQueue;
                    return 0;
                }
            }
        }
        if(strcmp(arr[i].key, "") == 0){
            strcpy(arr[i].key, key);
            arr[i].subs[0].pid = pid;
            arr[i].subs[0].clientQueue = clientQueue;
            return 0;
        }
    }
    return 1;
}

int delSubscriber(SubscriberStore *arr, char key[KEYSIZE], int pid){
    for (int i = 0; i < MAXKEYS; i++) {
        if(strcmp(arr[i].key, key) == 0){
            for (int j = 0; j < MAXSUBS; j++) {
                if (arr[i].subs[j].pid == 0)
                    return 1;

                if (arr[i].subs[j].pid == pid){
                    arr[i].subs[j].pid = 0;
                    arr[i].subs[j].clientQueue = 0;
                    for(int k = j; k < MAXSUBS - 1; k++){
                        arr[k] = arr[k + 1];
                    }

                    arr[i].subs[MAXSUBS - 1].pid = 0;
                    return 0;
                }
            }
        }
    }
    return 1;
}

//get subscribers for a key
int getSubscriber(SubscriberStore *arr, char* key, int* res){
    for (int i = 0; i < MAXKEYS; i++) {
        if(strcmp(arr[i].key, key) == 0){
            for (int j = 0; j < MAXSUBS; j++) {
                if (arr[i].subs[j].pid == 0)
                    return 1;

                res[j] = arr[i].subs[j].pid;
            }
            return 0;
        }
    }
    return 1;
}

