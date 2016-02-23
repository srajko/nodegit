#include "../include/thread_pool.h"
#include <uv.h>
#include <queue>

#include "../include/functions/sleep_for_ms.h"

struct work_t {
    work_cb callback;
    void *data;

    work_t(work_cb callback, void *data)
      : callback(callback), data(data) {

    }
};

std::queue<work_t> queue;
uv_mutex_t queue_mutex;

void spawn_thread_loop();
void run_libgit2_event_queue(void *);

void queue_work(work_cb callback, void *data)
{
  uv_mutex_lock(&queue_mutex);
  queue.push(work_t(callback, data));
  uv_mutex_unlock(&queue_mutex);
}

void initialize_thread_pool() {
  uv_mutex_init(&queue_mutex);

  for(int i=0; i<5; i++) {
    spawn_thread_loop();
  }
}

void spawn_thread_loop() {
  uv_thread_t libgit2_thread_id;
  uv_thread_create(&libgit2_thread_id, run_libgit2_event_queue, NULL);
}

void run_libgit2_event_queue(void *)
{
  // run, CPU, run
  for ( ; ; ) {
    sleep_for_ms(10);
    uv_mutex_lock(&queue_mutex);
    if(!queue.empty()) {
      work_t work = queue.front();
      queue.pop();
      uv_mutex_unlock(&queue_mutex);

      (*work.callback)(work.data);
    } else {
      uv_mutex_unlock(&queue_mutex);
    }
  }
}
