//
// Created by Widumaster on 03.04.2023.
//
#ifndef PRAKBS21_COMMANDSTRUCT_H
#define PRAKBS21_COMMANDSTRUCT_H

typedef enum commandKey_{

    CommandPUT,
    CommandGET,
    CommandDEL,
    CommandBEG,
    CommandEND,
    CommandSUB,

    CommandQUIT,

    NOCommand
}commandKey;


typedef struct CommandStruct_{

    char* input;
    commandKey commandType;
    char* commandKey;
    char* commandText;

}CommandStruct;

void CommandStructInitialize(CommandStruct* commandStruct);
void CommandStructReInit(CommandStruct* commandStruct);
unsigned char CommandStructConstruct(CommandStruct* commandStruct, char* input);
commandKey FilterCommandType(char* input);

#endif //PRAKBS21_COMMANDSTRUCT_H
