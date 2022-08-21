#include"defs.h"

#define EOF -1
#define BUFFER_SIZE 32



union Payload
{
        struct
        {
                unsigned char srcAddress;
                unsigned char destAddress;
                unsigned char data_[1];
        }content;
        unsigned char buffer[BUFFER_SIZE];
};

void AppInterface_init(char mode, int baud);
int AppInterface_readAppData(union Payload* p);
void AppInterface_writeAppData(union Payload* p);