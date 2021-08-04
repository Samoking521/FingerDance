#include "led_queue.h"
#include <stdio.h>

#define LED_QUEUE_MAXSIZE 10

LQNode LED_Queue[LED_QUEUE_MAXSIZE];
uint8_t front;
uint8_t rear;

void LED_Queue_Init()
{
    front = rear = 0;
}

bool LED_Queue_IsFull()
{
    return ((rear + 1) % LED_QUEUE_MAXSIZE == front);
}

void LED_Queue_Add(uint8_t *ledLine)
{
    if (LED_Queue_IsFull())
    {
        front = (front + 1) % LED_QUEUE_MAXSIZE;
        rear = (rear + 1) % LED_QUEUE_MAXSIZE;
        for (int i = 0; i < 4; i++)
            LED_Queue[rear].data[i] = ledLine[i];
    }
    else
    {
        rear = (rear + 1) % LED_QUEUE_MAXSIZE;
        for (int i = 0; i < 4; i++)
            LED_Queue[rear].data[i] = ledLine[i];
    }
}

LQNode* LED_Queue_GetFront()
{
    return &LED_Queue[(front + 1) % LED_QUEUE_MAXSIZE];
}

LQNode* LED_Queue_GetByIndex(int index)
{
    return &LED_Queue[(front + index) % LED_QUEUE_MAXSIZE];
}

void LED_Queue_Debug()
{
    int start = front;
    while (start != rear)
    {
        start = (start + 1) % LED_QUEUE_MAXSIZE;
        printf("queue index %d: %d\n", start, LED_Queue[start].data[0]);
    }
}