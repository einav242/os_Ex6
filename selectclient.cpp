#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <arpa/inet.h>

#define PORT 9038
#define ip "127.0.0.1"

int counter=0;

void *send(void *value)
{
	int sock=*(int*)value;
    char data[1024];
    while (1)
    {
		memset(data,0,strlen(data));
		fgets(data, 1024, stdin);
        int size= strlen(data)+1;
		printf("send the massege: %s\n",data);
		if (send(sock, data,size, 0) == -1)
        {
            perror("send");
        }
        if (strncmp(data,"EXIT",4) == 0)
        {
            break;
        }
    }
    return NULL;
}

void *recive(void *value)
{
	int sock=*(int*)value;
    char buffer[1024];
    while (1)
    {
		memset(buffer,0,strlen(buffer));
		if((recv(sock, buffer, 1024, 0))<=0)
		{
			break;
		}
        if (strncmp(buffer, "EXIT",4)==0)
        {
            continue;
        }
		printf("%d)new message: %s\n",counter,buffer);
		counter++;
    }
    return NULL;
}

int main()
{
    printf("to exit write: EXIT else enter your massege \n");
	int sock;
	struct sockaddr_in server_a;
    pthread_t thread1;
	pthread_t thread2;
	memset(&server_a, 0, sizeof(server_a));
    server_a.sin_family = AF_INET;
    server_a.sin_addr.s_addr = inet_addr(ip);
    server_a.sin_port = htons(PORT); 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        printf("\n Error: in socket creation \n"); 
        return -1;
    }

    if(connect(sock, (struct sockaddr *)&server_a, sizeof(server_a))<0)
	{
		printf("Failed: connect failed, Error code\n");
        close(sock);
        return -1;
	}

    pthread_create(&thread1, NULL, recive, &sock);
    pthread_create(&thread2, NULL, send, &sock);
    pthread_join(thread2, NULL);

    close(sock);
    printf("bye bye\n");
    return 0;
}