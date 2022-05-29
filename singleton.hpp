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
#include<assert.h>
using namespace std;

template <class T>
class Singleton
{
private:
    static Singleton *instance;
    T data;
    Singleton(T d)
    {
        data = d;
        pthread_mutex_lock(&mutex);
    }
    pthread_mutex_t mutex;

public:
    static Singleton *Instance(T d)
    {
        if (instance == NULL)
        {
            instance = new Singleton(d);
        }
        return instance;
    }
    void Destroy()
    {
        instance = NULL;
        delete instance;
        pthread_mutex_unlock(&mutex);
    }
};
template <class T>
Singleton<T> *Singleton<T>::instance = NULL;