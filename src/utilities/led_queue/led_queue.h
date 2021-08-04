#ifndef LED_QUEUE_H
#define LED_QUEUE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t data[4];
}LQNode;

void LED_Queue_Init();
bool LED_Queue_IsFull();
void LED_Queue_Add(uint8_t *ledLine);
LQNode* LED_Queue_GetFront();
LQNode* LED_Queue_GetByIndex(int index);
void LED_Queue_Debug();

#endif
