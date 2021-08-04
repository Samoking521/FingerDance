#ifndef ANS_QUEUE_H
#define ANS_QUEUE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint32_t tick;
    uint8_t data[4];
}AQNode;

void ANS_Queue_Init();
bool ANS_Queue_IsFull();
void ANS_Queue_Add(AQNode *ansLine);
AQNode* ANS_Queue_GetPre();
AQNode* ANS_Queue_GetCur();
void ANS_Queue_Debug();

#endif
