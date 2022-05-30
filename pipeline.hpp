#include"queue.hpp"
#include"active_object.hpp"
typedef struct pipeline
{
    queue *q;
    active_object *ao1;
    active_object *ao2;
    active_object *ao3;

} pipeline;