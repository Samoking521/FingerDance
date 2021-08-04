#include "ans_queue.h"
#include <stdio.h>

#define ANS_QUEUE_MAXSIZE 3

AQNode ANS_Queue[ANS_QUEUE_MAXSIZE];
uint8_t pre;
uint8_t cur;

void ANS_Queue_Init()
{
    pre = cur = 0;
}

bool ANS_Queue_IsFull()
{
    return ((cur + 1) % ANS_QUEUE_MAXSIZE == pre);
}

void ANS_Queue_Add(AQNode *ansLine)
{
    if (ANS_Queue_IsFull())
    {
        pre = (pre + 1) % ANS_QUEUE_MAXSIZE;
        cur = (cur + 1) % ANS_QUEUE_MAXSIZE;
        for (int i = 0; i < 4; i++)
        {
            ANS_Queue[cur].data[i] = ansLine->data[i];
        }
        ANS_Queue[cur].tick = ansLine->tick;
    }
    else
    {
        cur = (cur + 1) % ANS_QUEUE_MAXSIZE;
        for (int i = 0; i < 4; i++)
        {
            ANS_Queue[cur].data[i] = ansLine->data[i];
        }
        ANS_Queue[cur].tick = ansLine->tick;
    }
}

AQNode* ANS_Queue_GetPre()
{
    return &ANS_Queue[(pre + 1) % ANS_QUEUE_MAXSIZE];
}

AQNode* ANS_Queue_GetCur()
{
    return &ANS_Queue[cur % ANS_QUEUE_MAXSIZE];
}

void ANS_Queue_Debug()
{
    int start = pre;
    while (start != cur)
    {
        start = (start + 1) % ANS_QUEUE_MAXSIZE;
        printf("ans queue index %d: %d tick:%u\n", start, ANS_Queue[start].data[0], ANS_Queue[start].tick);
    }
}