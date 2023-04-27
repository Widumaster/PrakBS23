//
// Created by muragara on 4/25/23.
//

#ifndef PRAKBS23_SEMAPHORE_H
#define PRAKBS23_SEMAPHORE_H

typedef int semaphore;

void up (semaphore id, int elem);
void down (semaphore id, int elem);

#endif //PRAKBS23_SEMAPHORE_H
