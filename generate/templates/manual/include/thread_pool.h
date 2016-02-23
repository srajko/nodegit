#ifndef THREAD_POOL
#define THREAD_POOL

typedef void (*work_cb) (void *);

void initialize_thread_pool();
void queue_work(work_cb callback, void *data);

#endif
