//
// Created by Widumaster on 03.04.2023.
//
#include "commandStruct.h"
#include <stdlib.h>
#include "string.h"

void CommandStructInitialize(CommandStruct* commandStruct){
    commandStruct->input = NULL;
    commandStruct->commandType = NOCommand;
    commandStruct->commandKey = NULL;
    commandStruct->commandText = NULL;
}

unsigned char CommandStructConstruct(CommandStruct* commandStruct, char* input){

    int length = 0;
    const char sort = ' ';
    int sortCount = 0;
    int sortIndex[2] = {-1, -1};
    int stringLength = strnlen(input, 1024);
    char* commandString = 0;

    if(commandStruct->commandText != 0)
    {
        free(commandStruct->commandText);
        commandStruct->commandText = NULL;
        CommandStructInitialize(commandStruct);
    }

    commandStruct->commandType = FilterCommandType(input);

    commandStruct->commandText = calloc(stringLength + 1, sizeof(char));
    commandString = commandStruct->commandText;

    /*if (dataString == 0)
    {
        return 0;
    }*/

    memcpy(commandString, input, stringLength * sizeof(char));

    //PUT key value1
    for (; commandString[length] != '\0'  && sortCount < 2; length++)
    {
        char isSeperator = commandString[length] == sort;
        if (isSeperator)
        {
            sortIndex[sortCount++] = length;
            commandString[length] = '\0';
        }
    }

    if (sortIndex[0] != -1) commandStruct->commandKey = &commandString[sortIndex[0] + 1];

    if (sortIndex[1] != -1) commandStruct->commandText = &commandString[sortIndex[1] + 1];

    return 0;
}

commandKey FilterCommandType(char* input){


    if(memcmp("PUT ", input, 4) == 0)
        return CommandPUT;
    if(memcmp("GET ", input, 4) == 0)
        return CommandGET;
    if(memcmp("DEL ", input, 4) == 0)
        return CommandDEL;
    if(memcmp("BEG ", input, 4) == 0)
        return CommandBEG;
    if(memcmp("END ", input, 4) == 0)
        return CommandEND;
    if(memcmp("SUB ", input, 4) == 0)
        return CommandSUB;
    if(memcmp("QUIT", input, 4) == 0)
        return CommandQUIT;

    return NOCommand;
}