//
// Created by muragara on 4/25/23.
//

#ifndef PRAKBS23_CMDENUM_H
#define PRAKBS23_CMDENUM_H

enum CMD{PUT, GET, DEL, QUIT, BEG, END, ERR};

char* getCmdString(enum CMD cmd);
enum CMD getCmdValue(char* cmd);

#endif //PRAKBS23_CMDENUM_H
