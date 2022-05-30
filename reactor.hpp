#include<stdio.h>
#include <thread>
class Reactor
{
public:
    int file;
    pthread_t thread;
    void* (*func)(void *);
    Reactor()
    {

    }
    ~Reactor()
    {
        delete this;
    }
};
void* newReactor();
void InstallHandler(Reactor *r, void* (*func)(void *), int file);
void RemoverHandler(Reactor *r, int file);