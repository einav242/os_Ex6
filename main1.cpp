#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "queue.hpp"

#define PORT "3540"
#define BACKLOG 10
#define MAXSIZE 1024
#define NUMBER_OF_USER 10
pthread_t threads[NUMBER_OF_USER];

int digit(int n)
{
    int count = 0;
    if (n == 0)
    {
        return 1;
    }
    while (n > 0)
    {
        n = n / 10;
        count++;
    }
    return count;
}

/*******queue*********/
pthread_mutex_t mutex;
pthread_cond_t cond;

queue *createQ()
{
    queue *q = (queue *)malloc(sizeof(queue));
    q->front = (Node *)malloc(sizeof(Node));
    if (q->front == NULL)
    {
        printf("\nOVERFLOW\n");
        return NULL;
    }
    q->front = NULL;
    q->rear = (Node *)malloc(sizeof(Node));
    if (q->rear == NULL)
    {
        printf("\nOVERFLOW\n");
        return NULL;
    }
    q->rear = NULL;
    return q;
}
void destoryQ(queue *q)
{
    while (q->front != NULL)
    {
        Node *tmp = q->front;
        q->front = q->front->next;
        free(tmp);
    }
    free(q);
}
void enQ(queue *q, void *value)
{
    pthread_mutex_lock(&mutex);
    Node *n = (Node *)malloc(sizeof(Node));
    n->data = (char *)malloc(strlen((char*)value));
    strcpy((char*)n->data,(char*) value);
    n->next = NULL;
    if (q->front == NULL)
    {
        q->front = n;
        q->rear = n;
    }
    else
    {
        q->rear->next = n;
        q->rear = n;
    }
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);
}
void *deQ(queue *q)
{
    pthread_mutex_lock(&mutex);
    if (q->front == NULL)
    {
        printf("Underflow");
        pthread_cond_wait(&cond, &mutex);
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    else
    {
        Node *ptr = q->front;
        q->front = q->front->next;
        void *s = ptr->data;
        free(ptr);
        pthread_mutex_unlock(&mutex);
        return s;
    }
}

/*******queue*********/

/******active_object*******/
typedef struct active_object
{
    queue *q;
    void *(*func1)();
    void *(*func2)();
} active_object;

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

void destroyAO(active_object *ao)
{
    for(int j=0;j<NUMBER_OF_USER;j++)
    {
        if(threads[j]>0)
        {
            pthread_cancel(threads[j]);
        }
    }
    destoryQ(ao->q);
    free(ao);
}
/******active_object*******/

/******pipeline***********/

typedef struct pipeline
{
    queue *q;
    active_object *ao1;
    active_object *ao2;
    active_object *ao3;

} pipeline;

/******pipeline***********/

queue *q1;
queue *q2;
queue *q3;
pipeline *my_pipe;
active_object *ao1;
active_object *ao2;
active_object *ao3;

/*****function-pipeline*****/
void *caesar_cipher()
{
    char data[strlen((char *)q1->front->data)];
    strcpy(data, (char *)q1->front->data);
    for (unsigned int i = 0; i < strlen(data); i++)
    {
        if (data[i] >= 'A' && data[i] <= 'Z')
        {
            if (data[i] == 'Z')
            {
                data[i] = 'A';
            }
            else
            {
                data[i] = data[i] + 1;
            }
        }
        else if (data[i] >= 'a' && data[i] <= 'z')
        {
            if (data[i] == 'z')
            {
                data[i] = 'a';
            }
            else
            {
                data[i] = data[i] + 1;
            }
        }
    }
    q1->front->data = data;
    return NULL;
}

void *Up_Low()
{
    char data[strlen((char *)q2->front->data)];
    strcpy(data, (char *)q2->front->data);
    for (unsigned int i = 0; i < strlen(data); i++)
    {
        if (data[i] >= 'A' && data[i] <= 'Z')
        {
            data[i] = data[i] + ('a' - 'A');
        }
        else if (data[i] >= 'a' && data[i] <= 'z')
        {
            data[i] = data[i] - ('a' - 'A');
        }
    }
    q2->front->data = data;
    return NULL;
}

void *sendm()
{
    char data[strlen((char *)q3->front->data)];
    strcpy(data, (char *)q3->front->data);
    char msg[1024];
    memset(msg, 0, strlen(msg));
    int i = 0;
    while (data[i] != ' ')
    {
        msg[i] = data[i];
        i++;
    }
    int j = 0;
    char s[1024];
    memset(s, 0, strlen(s));
    while (i < strlen(data))
    {
        s[j] = data[i];
        j++;
        i++;
    }
    int sock = (int)strtol(s, (char **)NULL, 10);
    msg[strlen(msg)-1]='\0';
    if (send(sock, msg, 1023, 0) == -1)
	{
		perror("send");
	}
    return NULL;
}

void *after_caesar_cipher()
{
    if (q1->front != NULL)
    {
        char data[1024];
        memset(data, 0, strlen(data));
        strcpy(data, (char *)q1->front->data);
        enQ(q2, data);
    }
    return NULL;
}

void *after_Up_Low()
{
    if (q2->front != NULL)
    {
        char data[1024];
        memset(data, 0, strlen(data));
        strcpy(data, (char *)q2->front->data);
        enQ(q3, &data);
    }
    return NULL;
}

void *after_sandm()
{
    return NULL;
}

/*****function-pipeline*****/

/****fuction for the server*****/
void *func(void *value)
{
    int sock = *(int *)value;
    char data[1024];
    memset(data, 0, strlen(data));
    int check;
    while (1)
    {
        memset(data,0,strlen(data));
        if ((check = recv(sock, data, 1023, 0)) == -1)
        {
            printf("User logged out\n");
            close(sock);
            return NULL;
        }
        if (check==0)
        {
            printf("User logged out\n");
            close(sock);
            return NULL;
        }
        
        data[strlen(data)] = '\0';
        if (strncmp(data, "EXIT", 5) == 0)
        {
            break;
        }
        else 
        {
            char add_data[2]=" ";
            char s[digit(sock)];
            sprintf(s, "%d", sock);
            strcat(data,add_data);
            strcat(data,s);
            if(q1->front==NULL)
            {
                enQ(q1,data);
                my_pipe->ao1=newAO(q1, &caesar_cipher, &after_caesar_cipher);
                my_pipe->ao2=newAO(q2, &Up_Low, &after_Up_Low);
                my_pipe->ao3=newAO(q3, &sendm, &after_sandm);
                my_pipe->q=q1;
            }
            else
            {
                enQ(q1,data);
            }
        }
    }
    return NULL;
    
}

void sig_handler(int sig)
{
    if(my_pipe->ao1!=NULL)
    {
        destroyAO(my_pipe->ao1);
    }
    if(my_pipe->ao2!=NULL)
    {
        destroyAO(my_pipe->ao2);
    }
    if(my_pipe->ao3!=NULL)
    {
        destroyAO(my_pipe->ao3);
    }
	printf("\nbye bye\n");
	exit(0);
}

void sigchld_handler(int s)
{
	(void)s; 


	int saved_errno = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0)
		;

	errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

/****fuction for the server*****/

int main()
{
    q1 = createQ();
    q2 = createQ();
    q3 = createQ();
    my_pipe = (pipeline *)malloc(sizeof(pipeline)); 
    printf("server:to exit press ctrl+c or ctrl+z\n");
	int sockfd, new_fd; 
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
					   sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo);

	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; 
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");
    int i = 0;
	signal(SIGINT, sig_handler);
	signal(SIGTSTP,sig_handler);
	while (1)
	{ 
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1)
		{
			perror("accept");
			continue;
		}
		inet_ntop(their_addr.ss_family,
				  get_in_addr((struct sockaddr *)&their_addr),
				  s, sizeof s);
		printf("server: got connection from %s\n", s);
		if (pthread_create(&threads[i], NULL, &func, &new_fd) != 0)
        {
            printf("Failed to create thread\n");
        }
		i++;
        if (i >= NUMBER_OF_USER)
        {
            for(int j=0;j<NUMBER_OF_USER;j++)
            {
                pthread_join(threads[j], NULL);
            }
            i = 0;
        }
	}

    return 0;
}