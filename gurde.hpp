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
#include <thread>
#include <mutex>
using namespace std;


void* global_pointer;
pthread_mutex_t mutex1;
int check=0;
class gurde
{

public:
    gurde();

    ~gurde();
};

gurde::gurde()
{
    pthread_mutex_lock(&mutex1);
}

gurde::~gurde()
{
    pthread_mutex_unlock(&mutex1);
}