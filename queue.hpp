#include <pthread.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
#pragma once


typedef struct Node {
    void* data;
    struct Node *next;
}Node;

typedef struct queue
{
    Node* front;
    Node* rear;
}queue;


queue* createQ();
void destoryQ(queue* q);
void enQ(queue* q,void*);
void* deQ(queue* q);


