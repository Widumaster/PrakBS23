//
// Created by muragara on 4/13/23.
//

#ifndef PRAKBS23_INPUTHANDLER_H
#define PRAKBS23_INPUTHANDLER_H

#include "common.h"
#include "subStore.h"

int handleInput(Message *messageArr, SubscriberStore *subArr, char* in, int qid, int clientQueue);
void handleBlockedInput( Message *messageArr, char* in, char* out);

#endif //PRAKBS23_INPUTHANDLER_H
