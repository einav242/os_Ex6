#include "gurde.hpp"
/*is it possible to use such a guard to protect strtok? No, 
because each thread may enter in a different order than expected to the the function*/
#define NUMBER_OF_USER 100


void* func()
{
    gurde g;
    global_pointer=malloc(sizeof(int));
    check++;
    cout<<"global pointer: "<<global_pointer<<" \n"; //shuold print 100 difrrent adress
    return global_pointer;
}

int main()
{
    thread threads[NUMBER_OF_USER];
    unsigned int i=0;
    while(i<NUMBER_OF_USER)
    {
        threads[i]=thread(func);
        i++;
    }
    i=0;
    while(i<NUMBER_OF_USER)
    {
        threads[i].join();
        i++;
    }
    cout<<"check="<<check<<"\n"; //shuold print 100
    return 0;
}
