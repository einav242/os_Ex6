#include"queue.hpp"
#include<thread>
#define num 10
#pragma once
typedef struct active_object
{
    queue *q;
    void *(*func1)();
    void *(*func2)();
} active_object;

active_object *newAO(queue *q, void *(*func1)(), void *(*func2)());
void destroyAO(active_object *ao,pthread_t threads[num]);