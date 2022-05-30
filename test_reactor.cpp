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
#define PORT "9036" 
#define MAXDATASIZE 100 
#define ip "127.0.0.1"
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main()
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
		return 1;
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
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			  s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo);

    char buffer[1024];
    char data1[1024] = "bar";
    char data2[1024] = "einav";
    char data3[1024] = "bar and einav";
	char data4[1024] = "EXIT";
    if (send(sockfd, data1, strlen(data1) + 1, 0) == -1)
    {
        perror("send");
    }
	if ((recv(sockfd, buffer, 1024, 0)) <= 0)
    {
        perror("recv");
        exit(1);
    }
    assert(strncmp(buffer, "bar", 4) == 0);
	printf("Test 1 passed\n");
    sleep(1);

    if (send(sockfd, data2, strlen(data2) + 1, 0) == -1)
    {
        perror("send");
    }
    if ((recv(sockfd, buffer, 1024, 0)) <= 0)
    {
        perror("recv");
        exit(1);
    }
    assert(strncmp(buffer, "einav", 6) == 0);
    printf("Test 2 passed\n");
	sleep(1);

	if (send(sockfd, data3, strlen(data3) + 1, 0) == -1)
    {
        perror("send");
    }
    if ((recv(sockfd, buffer, 1024, 0)) <= 0)
    {
        perror("recv");
        exit(1);
    }
    assert(strncmp(buffer, "bar and einav", 14) == 0);
    printf("Test 3 passed\n");

	if (send(sockfd, data4, strlen(data4) + 1, 0) == -1)
    {
        perror("send");
    }

	close(sockfd);
    return 0;
}