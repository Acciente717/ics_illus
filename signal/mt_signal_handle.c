#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

void sigtstp_handler(int sig)
{
    printf("sigtstp handled\n");
}

void sigint_handler(int sig)
{
    printf("sigint handled\n");
    pthread_exit(NULL);
}

void *idle(void *vnum)
{
    long num = (long)vnum;
    while (1)
    {
        pause();
        printf("handled by %ld\n", num);
    }
    return NULL;
}

int main()
{
    signal(SIGTSTP, sigtstp_handler);
    signal(SIGINT, sigint_handler);
    pthread_t a, b, c, d;
    pthread_create(&a, NULL, idle, (void*)1);
    pthread_create(&b, NULL, idle, (void*)2);
    pthread_create(&c, NULL, idle, (void*)3);
    pthread_create(&d, NULL, idle, (void*)4);

    pthread_exit(NULL);

    return 0;
}