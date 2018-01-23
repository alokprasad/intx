#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
sem_t semaphore1;
sem_t semaphore2;
void *EVEN(void *param);
void *ODD(void *param);
int main()
{
        pthread_t etid,otid;

        sem_init(&semaphore1,0,1);
        sem_init(&semaphore2,0,0);
        pthread_create(&etid,NULL,EVEN,NULL);
        pthread_create(&otid,NULL,ODD,NULL);

        pthread_join(etid,NULL);
        pthread_join(otid,NULL);
}

void *EVEN(void *param)
{
        int x=0;
        int i;
        for(i=0;i<20;i++)
        {
                sem_wait(&semaphore1);
                printf("%d ",x);
                x=x+2;
                sem_post(&semaphore2);
        }
}

void *ODD(void *param)
{
        int x=1;
        int i;
        for(i=0;i<20;i++)
        {
                sem_wait(&semaphore2);

                printf("%d ",x);
                x=x+2;
                sem_post(&semaphore1);
        }
}
