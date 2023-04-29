//
// Created by muragara on 4/27/23.
//

#include <string.h>
#include "subStore.h"

int putSubscriber(SubscriberStore *arr, char key[KEYSIZE], int pid){
    for(int i = 0; i < MAXKEYS; i++){
        if(strcmp(arr[i].key, key) == 0){
            for (int j = 0; j < MAXSUBS; j++) {
                if (arr[i].pids[j] == pid)
                    return 1;
                if (arr[i].pids[j] == 0){
                    arr[i].pids[j] = pid;
                }
            }
        }
        if(strcmp(arr[i].key, "") == 0){
            strcpy(arr[i].key, key);
            arr[i].pids[0] = pid;
            return 0;
        }
    }
    return 1;
}

int delSubscriber(SubscriberStore *arr, char key[KEYSIZE], int pid){
    for (int i = 0; i < MAXKEYS; i++) {
        if(strcmp(arr[i].key, key) == 0){
            for (int j = 0; j < MAXSUBS; j++) {
                if (arr[i].pids[j] == 0)
                    return 1;

                if (arr[i].pids[j] == pid){
                    arr[i].pids[j] = 0;
                    for(int k = j; k < MAXSUBS - 1; k++){
                        arr[k] = arr[k + 1];
                    }

                    arr[i].pids[MAXSUBS - 1] = 0;
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
                if (arr[i].pids[j] == 0)
                    return 1;

                res[j] = arr[i].pids[j];
            }
            return 0;
        }
    }
    return 1;
}

