#ifndef __THREAD_H
#define __THREAD_H

typedef struct RuntimeThread {
    char **args;
} RuntimeThread;

void wait_for_pid(int);
int run_async(RuntimeThread *, int *, int *);

#endif
