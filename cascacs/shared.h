#ifndef SHARED_H
#define SHARED_H

#include <pthread.h>

extern int shared_variable;
extern pthread_mutex_t pthread_mutex;

void* writer_thread(void*);
void* reader_thread(void*);
#endif // SHARED_H
