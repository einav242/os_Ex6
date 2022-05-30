#include"queue.hpp"
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
