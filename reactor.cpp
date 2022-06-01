

#include"reactor.hpp"
using namespace std;

Reactor* newReactor()
{
    Reactor reactor=Reactor();
    Reactor *ans=&reactor;
    return ans;
}
void InstallHandler(Reactor *r, void* (*func)(void *), int file)
{
    r->funcs[r->index]=func;
    r->pfds[r->index].fd=file;
    r->pfds[r->index].events=POLLIN;
    if(r->index==r->size)
    {
        r->size*=2;
        r->pfds = (struct pollfd*)realloc(r->pfds, sizeof(*r->pfds) * (r->size));
    }
    r->index++;
    if (pthread_create(&r->thread, NULL, func,(void*)&r->pfds[r->index-1].fd) != 0)
    {
        printf("Failed to create thread\n");
    }
}
void RemoverHandler(Reactor *r, int file)
{
    int ind=-1;
    for(int i=0;i<r->index;i++)
    {
        if(r->pfds[i].fd==file)
        {
            ind=i;
            break;
        }
    }
    if(ind!=-1)
    {
        r->pfds[ind] = r->pfds[r->index - 1];
        r->index--;
    }
}