#include "gurde.hpp"
/*is it possible to use such a guard to protect strtok? No, 
because each thread may enter in a different order than expected to the the function*/
#define NUMBER_OF_USER 100

pthread_mutex_t mutex1;

gurde::gurde()
{
    pthread_mutex_lock(&mutex1);
}

gurde::~gurde()
{
    pthread_mutex_unlock(&mutex1);
}
