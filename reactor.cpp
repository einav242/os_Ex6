
#include"reactor.hpp"
using namespace std;

void* newReactor()
{
    Reactor* ans=(Reactor*)malloc(sizeof(Reactor));
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