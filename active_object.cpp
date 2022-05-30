#include"active_object.hpp"
#include"queue.hpp"
active_object *newAO(queue *q, void *(*func1)(), void *(*func2)())
{
    active_object *ans = (active_object *)malloc(sizeof(active_object));
    ans->q = q;
    ans->func1 = func1;
    ans->func2 = func2;
    Node *n = q->front;
    while (n != NULL)
    {
        n = n->next;
        func1();
        func2();
        deQ(q);
    }
    return ans;
}

void destroyAO(active_object *ao,pthread_t threads[num])
{
    for(int j=0;j<num;j++)
    {
        if(threads[j]>0)
        {
            pthread_cancel(threads[j]);
        }
    }
    destoryQ(ao->q);
    free(ao);
}