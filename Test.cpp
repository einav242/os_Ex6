#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <string.h>
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
#include <assert.h>
#include "gurde.hpp"
#include "singleton.hpp"
using namespace std;
#define NUMBER_OF_USER 10

#define PORT "9038" 

#define MAXDATASIZE 100 
#define ip "127.0.0.1"

void* global_pointer;
int check=0;


void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}


void* func_tread1(void* v)
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(ip, PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return NULL;
	}

	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1)
		{
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL)
	{
		fprintf(stderr, "client: failed to connect\n");
		return NULL;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			  s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo);
	char buffer[1024];
    char data1[1024] = "bar";
	if (send(sockfd, data1, strlen(data1) + 1, 0) == -1)
    {
        perror("send");
    }
	if ((recv(sockfd, buffer, 1024, 0)) <= 0)
    {
        perror("recv");
        exit(1);
    }
    assert(strncmp(buffer, "einav", 4) == 0);
	printf("Test 2 passed\n");
	return NULL;
}

void* func_tread2(void* v)
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(ip, PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return NULL;
	}

	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1)
		{
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL)
	{
		fprintf(stderr, "client: failed to connect\n");
		return NULL;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			  s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo);
	char buffer[1024];
	memset(buffer,0,strlen(buffer));
    char data1[1024] = "einav";
	if ((recv(sockfd, buffer, 1024, 0)) <= 0)
    {
        perror("recv");
        exit(1);
    }
    assert(strncmp(buffer, "bar", 3) == 0);
	printf("Test 1 passed\n");
	if (send(sockfd, data1, strlen(data1) + 1, 0) == -1)
    {
        perror("send");
    }
	return NULL;

}

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
    cout << "****gurde test**** \n";
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
    cout << "*****end gurde test****\n";
    cout << "***singelton test**** \n";
    FILE *file;
    Singleton<FILE *> *s1 = Singleton<FILE *>::Instance(file);
    Singleton<FILE *> *s2 = Singleton<FILE *>::Instance(file);
    Singleton<FILE *> *s3 = Singleton<FILE *>::Instance(file);
    assert(s1 == s2);
    assert(s3 == s2);
    cout << "singelton1= " << s1 << "\n";
    cout << "singelton2= " << s2 << "\n";
    cout << "singelton3= " << s3 << "\n";
    cout << "the same address!\n";
    cout << "Destroy:\n";
    s1->Destroy();
    Singleton<FILE *> *s4 = Singleton<FILE *>::Instance(file);
    cout << "singelton4= " << s4 << "\n";
    assert(s1 != s4);
    assert(s2 != s4);
    assert(s3 != s4);
    cout << "diffrent address!\n";
    cout << "****end singelton test****\n\n";

     cout << "***reactor test**** \n";

 	pthread_t thread1;
	pthread_t thread2;
	pthread_create(&thread1, NULL, func_tread1,NULL);
    pthread_create(&thread2, NULL, func_tread2, NULL);
    pthread_join(thread1, NULL);

	cout << "****end reactor test****\n\n";
    return 0;
}