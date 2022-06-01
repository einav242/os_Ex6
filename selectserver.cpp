// /*
// ** pollserver.c -- a cheezy multiperson chat server
// */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include "reactor.hpp"
#include <pthread.h>

#define PORT "9038" 
int fd_count;
struct pollfd *pfds;
int listener;
Reactor r;


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}


int get_listener_socket(void)
{
    int listener; // Listening socket descriptor
    int yes = 1;  // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0)
    {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0)
        {
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(listener);
            continue;
        }

        break;
    }

    freeaddrinfo(ai); // All done with this

    // If we got here, it means we didn't get bound
    if (p == NULL)
    {
        return -1;
    }

    // Listen
    if (listen(listener, 10) == -1)
    {
        return -1;
    }

    return listener;
}


void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size)
    {
        *fd_size *= 2; // Double it

        *pfds = (pollfd *)realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}


void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count - 1];

    (*fd_count)--;
}

void *func(void *value)
{
    int sock = *(int *)value;
    char buf[256]; 
    while (1)
    {
        memset(buf,0,strlen(buf));
        int nbytes = recv(sock, buf, sizeof buf, 0);
        int sender_fd = sock;
        if (nbytes <= 0)
        {
            if (nbytes == 0)
            {
                RemoverHandler(&r,sock);
                printf("pollserver: socket %d hung up\n", sender_fd);
            }
            else
            {
                printf("User logged out\n");
                break;
            }

            close(sock); 
        }
        else
        {
            for (int j = 0; j < fd_count + 1; j++)
            {
                int dest_fd = pfds[j].fd;
                if (dest_fd != listener && dest_fd != sock)
                {
                    send(dest_fd, buf,strlen(buf), 0);
                }
            }
        }
    }
    return NULL;
}

void sig_handler(int sig)
{
	printf("\nbye bye\n");
	exit(0);
}

int main(void)
{
    printf("server:to exit press ctrl+c or ctrl+z\n");
    signal(SIGINT, sig_handler);
	signal(SIGTSTP,sig_handler);
    int newfd;                         
    struct sockaddr_storage remoteaddr; 
    socklen_t addrlen;
    char remoteIP[INET6_ADDRSTRLEN];
    fd_count = 0;
    int fd_size = 5;
    pfds = (pollfd *)malloc(sizeof *pfds * fd_size);

    listener = get_listener_socket();

    if (listener == -1)
    {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    pfds[0].fd = listener;
    pfds[0].events = POLLIN; 
    fd_count = 1;           

    for (;;)
    {
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1)
        {
            perror("poll");
            exit(1);
        }

        for (int i = 0; i < fd_count; i++)
        {

            if (pfds[i].revents & POLLIN)
            { 

                if (pfds[i].fd == listener)
                {

                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                                   (struct sockaddr *)&remoteaddr,
                                   &addrlen);

                    if (newfd == -1)
                    {
                        perror("accept");
                    }
                    else
                    {
                        add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

                        printf("pollserver: new connection from %s on "
                               "socket %d\n",
                               inet_ntop(remoteaddr.ss_family,
                                         get_in_addr((struct sockaddr *)&remoteaddr),
                                         remoteIP, INET6_ADDRSTRLEN),
                               newfd);
                        r=Reactor();
                        InstallHandler(&r, &func, newfd);
                    }
                }
            }
        }
    }

    return 0;
}