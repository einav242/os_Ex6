#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <iostream>
#include <mutex>
#include"reactor.hpp"
using namespace std;


void* newReactor()
{
    Reactor* ans=new Reactor();

    return ans;
}
void InstallHandler(Reactor *r, void* (*func)(void *), int file)
{
    r->file=file;
    r->func=func;
     if (pthread_create(&r->thread, NULL, r->func,(void*)&r->file) != 0)
    {
        printf("Failed to create thread\n");
    }
}
void RemoverHandler(Reactor *r, int file)
{
    r->file=0;
    r->func=NULL;
    pthread_cancel(r->thread);
    delete r; 
}