
#include "gurde.hpp"
#include <assert.h>
#define NUMBER_OF_USER 10
void* global_pointer;
int check=0;

void *func1()
{
    gurde g;
    global_pointer = malloc(sizeof(int));
    check++;
    cout << check << ") global pointer: " << global_pointer << " \n"; //shuold print 100 difrrent adress
    return global_pointer;
}

int main()
{
    thread threads[NUMBER_OF_USER];
    unsigned int i = 0;
    while (i < NUMBER_OF_USER)
    {
        threads[i] = thread(func1);
        i++;
    }
    i = 0;
    while (i < NUMBER_OF_USER)
    {
        threads[i].join();
        i++;
    }
    cout << "check=" << check << "\n"; //shuold print 100
    assert(check == 10);
}