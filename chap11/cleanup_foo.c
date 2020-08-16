/*
    这是man pthread_cleanup_push中的程序例子
*/
#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static int done = 0;
static int cleanup_pop_arg = 0;
static int cnt = 0;

static void
cleanup_handler(void* arg)
{
    printf("Called clean-up handler\n");
    cnt = 0;
}

static void*
thread_start(void* arg)
{
    time_t start, curr;

    printf("New thread started\n");

    pthread_cleanup_push(cleanup_handler, NULL);

    curr = start = time(NULL);

    while (!done) {
        //printf("done = %d\n", done);
        pthread_testcancel();           /* A cancellation point */
        if (curr < time(NULL)) {        /*因为time返回的是秒，所以在一秒内只有
                                          一次使得time返回的值大于上一次记录的curr*/
            curr = time(NULL);
            printf("cnt = %d\n", cnt);  /* A cancellation point */
            cnt++;
        }
    }
    printf("done = %d\n", done);
    pthread_cleanup_pop(cleanup_pop_arg);
    return NULL;
}

int
main(int argc, char* argv[])
{
    pthread_t thr;
    int s;
    void* res;

    s = pthread_create(&thr, NULL, thread_start, NULL);
    if (s != 0)
        handle_error_en(s, "pthread_create");

    sleep(2);           /* Allow new thread to run a while */

    /*
    * 要么参数多于1：让线程运行2秒，然后置done为1退出并执行cleanup
    * 要么参数为1：执行pthread_cancel取消该线程，然后自动执行cleanup
    */
    if (argc > 1) {
        if (argc > 2)
            cleanup_pop_arg = atoi(argv[2]);
        done = 1;

    }
    else {
        printf("Canceling thread\n");
        s = pthread_cancel(thr);
        if (s != 0)
            handle_error_en(s, "pthread_cancel");
    }

    s = pthread_join(thr, &res);
    if (s != 0)
        handle_error_en(s, "pthread_join");

    if (res == PTHREAD_CANCELED)
        printf("Thread was canceled; cnt = %d\n", cnt);
    else
        printf("Thread terminated normally; cnt = %d\n", cnt);
    exit(EXIT_SUCCESS);
}