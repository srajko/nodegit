#ifndef ASYNC_LIBGIT2_QUEUE_WORKER_H
#define ASYNC_LIBGIT2_QUEUE_WORKER_H

#include <nan.h>
#include <uv.h>
#include "../include/thread_pool.h"

NAN_INLINE void DoNothing(uv_work_t* req) {
  // printf("DoNothing\n");
}

// Frees the uv_async_t handle
NAN_INLINE void AsyncLibgit2Free(uv_handle_t* uv_async) {
  // freeing the handle can crash the app... perhaps libuv needs it
  // after the async callback returns, in which case we can't free it from
  // within the async callback...
  // free(uv_async);
}

// Runs WorkComplete of the scheduled AsyncWorker,
// and destroys it.  This is run in the uv_default_loop event loop.
NAN_INLINE void AsyncLibgit2Complete (uv_work_t* req, int status) {
  // printf("AsyncLibgit2Complete\n");
  Nan::AsyncWorker *worker = static_cast<Nan::AsyncWorker*>(req->data);
  worker->WorkComplete();
  worker->Destroy();
  // it doesn't look like this is a good place to close the handle
  // - it can cause the tests to hang - perhaps libuv expects it to be
  // upen throughout the callback
  // uv_close((uv_handle_t *)uv_async, AsyncLibgit2Free);
}

// Runs Execute of the scheduled AyncWorker on the dedicated libgit2 thread /
// event loop, and schedules the WorkComplete callback to run on the
// uv_default_loop event loop
NAN_INLINE void AsyncLibgit2Execute (void *vworker) {
  // printf("AsyncLibgit2Execute\n");
  Nan::AsyncWorker *worker = static_cast<Nan::AsyncWorker*>(vworker);
  worker->Execute();
  // it doesn't look like this is a good place to clean up the handle
  // - it can cause the tests to hang - perhaps libuv expects it to be
  // upen throughout the callback
  // uv_close((uv_handle_t *)uv_async, AsyncLibgit2Free);

  // maybe we can reuse the first handle?  not sure, making a new one
  uv_work_t *req = (uv_work_t *)malloc(sizeof(uv_work_t));
  req->data = worker;
  uv_queue_work(uv_default_loop(), req, DoNothing, AsyncLibgit2Complete);
}

// Schedules the AsyncWorker to run on the dedicated libgit2 thread / event loop.
NAN_INLINE void AsyncLibgit2QueueWorker (Nan::AsyncWorker* worker) {
  // printf("AsyncLibgit2QueueWorker\n");
  queue_work(AsyncLibgit2Execute, worker);
}

#endif
