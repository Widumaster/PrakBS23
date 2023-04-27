//
// Created by muragara on 4/25/23.
//

#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Semaphore.h"

void up (semaphore id, int elem) {
    int t;
    struct sembuf sem_up;
    sem_up.sem_num = elem;
    sem_up.sem_op = +1;
    sem_up.sem_flg = 0;
    printf ("Semaphor up   %i %i: %i. %i\n", id, elem, semctl(id, elem, GETVAL), getpid());
    t = semop (id, &sem_up, 1);
    if (t < 0) {
        perror ("up");
        exit(1);
    }
}


void down (semaphore id, int elem) {
    struct sembuf sem_down;
    sem_down.sem_num = elem;
    sem_down.sem_op = -1;
    sem_down.sem_flg = 0;
    printf ("Semaphor down %i %i: %i. %i\n", id, elem, semctl(id, elem, GETVAL), getpid()); fflush(stdout);
    int t = semop (id, &sem_down, 1);
    if (t < 0) {
        perror ("down");
        exit(1);
    }
}
