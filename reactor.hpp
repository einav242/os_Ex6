#pragma once
#include <iostream>
#include <pthread.h>
#include <poll.h>

typedef void *(*functions)(void *);

class Reactor
{
public:
    pthread_t thread;
    functions *funcs;
    int index;
    int size;
    struct pollfd *pfds;
    Reactor()
    {
        this->funcs = (functions *)malloc(sizeof(functions) * 100);
        this->pfds = (struct pollfd *)malloc(sizeof *pfds * 100);
        this->index = 0;
        this->size = 100;
    }
    ~Reactor()
    {
    }
};

Reactor* newReactor();
void InstallHandler(Reactor* reactor, void*(func)(void*), int file_des);
void RemoverHandler(Reactor *r, int file);