#ifndef SHARED_H
#define SHARED_H

#include <pthread.h>

extern int shared_variable;
extern int respuesta_recibida;
extern pthread_mutex_t pthread_mutex;
extern pthread_cond_t condition;

void* writer_thread(void*);
void* reader_thread(void*);
#endif // SHARED_H
