#include "CommandTerminalInterface.h"

int myStrCmp(const char* str1, const char* str2)
{
    const char* p1 = str1;
    const char* p2 = str2;

    while (*p1 != '\0' && *p1 == *p2) {
        p1++;
        p2++;
    }
  
    return (*p1 - *p2 == 0) ? 1 : 0;
}

int myStrLen(const char* str)
{
    const char* p = str;

    while (*p != '\0') {
        p++;
    }
  
    return (p - str); 
}

char* myStrCpy(const char* original, char* copy)
{
    const char* p1 = original;
    char* p2 = copy;

    while (*p1 != '\0') {
        *p2++ = *p1++;
    }

    *p2 = '\0';

    return copy;
}

uint8_t getFieldCount(USER_DATA* data)
{
    return data->fieldCount;
}

// field 0- command, 1- 1st argument
char* getFieldString(USER_DATA* data, uint8_t fieldNumber)
{
    if (fieldNumber >= data->fieldCount) {
        return '\0';
    }
    
    uint8_t i = data->fieldPosition[fieldNumber];
    
    return &(data->buffer[i]);
}

// 0-indexed, convert numberic str to uint32_t
uint32_t getFieldInteger(USER_DATA* data, uint8_t fieldNumber)
{
    if (fieldNumber >= data->fieldCount) {
        return 0;
    }
  
    if (data->fieldType[fieldNumber] == 'n') {
        uint8_t i = data->fieldPosition[fieldNumber];
        char* str = &(data->buffer[i]);
        uint32_t val = 0;

        while (*str >= '0' && *str <= '9') {
            val = (val * 10) + (*str - '0');
            str++;
        }

        return val;
    }
   
    return 0;
}

bool isCommand(USER_DATA* data, const char* strCommand, uint8_t minArguments)
{
    if (data->fieldCount == 0) {
        return false;
    }
    
    char* str = getFieldString(data, 0);

    if (myStrCmp(str, strCommand)) {
        if ((data->fieldCount - 1) >= minArguments) return 1;
    }

    return 0;
}

// process string in-place
// hypten, comma, period is not processed
void parseFields(USER_DATA* data) 
{
    uint32_t i;
    uint8_t prevField = 'd';
    //data->fieldCount = 0;

    char readChar;

    for (i = 0; i < MAX_CHARS; i++) {
        readChar = data->buffer[i];
  
        if (readChar == '\0') return;
        // ALPHA & '&'
        if ((readChar >= 'a' && readChar <= 'z') || 
           (readChar >= 'A' && readChar <= 'Z') ||
           (readChar == '&')) {
            if (prevField != 'a') {
                if (data->fieldCount < MAX_FIELDS) {
                    data->fieldType[data->fieldCount] = 'a';
                    data->fieldPosition[data->fieldCount++] = i;
                }
                prevField = 'a';
            }
        } // NUMERIC
        else if (readChar >= '0' && readChar <= '9') {
            if (prevField != 'n') {
                if (data->fieldCount < MAX_FIELDS) {
                    data->fieldType[data->fieldCount] = 'n';
                    data->fieldPosition[data->fieldCount++] = i;
                }
                prevField = 'n';
            }
        } // DELIMETER
        else {
            if (prevField != 'd') {
                data->buffer[i] = '\0';
                prevField = 'd';
            }   
        }
    }

    return ;
}
