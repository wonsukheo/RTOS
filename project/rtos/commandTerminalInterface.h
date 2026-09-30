#ifndef COMMANDTERMINALINTERFACE_H_
#define COMMANDTERMINALINTERFACE_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_CHARS 80
#define MAX_FIELDS 5

typedef struct _USER_DATA {
    char buffer[MAX_CHARS];
    uint8_t fieldCount;
    uint8_t fieldPosition[MAX_FIELDS];
    char fieldType[MAX_FIELDS];
} USER_DATA;

int myStrCmp(const char* str1, const char* str2);
int myStrLen(const char* str);
char* myStrCpy(const char* origin, char* copy);

void parseFields(USER_DATA* data);
uint8_t getFieldCount(USER_DATA* data);
char* getFieldString(USER_DATA* data, uint8_t fieldNumber);
uint32_t getFieldInteger(USER_DATA* data, uint8_t fieldNumber);
bool isCommand(USER_DATA* data, const char strCommand[], uint8_t minArguments);

#endif 
