//
// Created by muragara on 4/25/23.
//

#include <string.h>
#include <stdio.h>
#include "cmdEnum.h"

char* getCmdString(enum CMD cmd){
    switch (cmd) {
        case PUT: return "PUT";
        case GET: return "GET";
        case DEL: return "DEL";
        case QUIT: return "QUIT";
        case BEG: return "BEG";
        case END: return "END";
        case SUB: return "SUB";
        default: return "ERR";
    }
}

enum CMD getCmdValue(char* cmd){
    if (strcmp(cmd, "PUT") == 0) {
        return PUT;
    }
    else if (strcmp(cmd, "GET") == 0) {
        return GET;
    }
    else if (strcmp(cmd, "DEL") == 0) {
        return DEL;
    }
    else if (strncmp(cmd, "QUIT", 4) == 0) {
        return QUIT;
    }
    else if (strncmp(cmd, "BEG", 3) == 0) {
        return BEG;
    }
    else if (strncmp(cmd, "END", 3) == 0) {
        return END;
    }
    else if(strncmp(cmd, "SUB", 3) == 0){
        return SUB;
    }
    else {
        return ERR;
    }
}