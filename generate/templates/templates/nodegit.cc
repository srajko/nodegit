// This is a generated file, modify: generate/templates/nodegit.cc.
#include <v8.h>
#include <node.h>
#include <git2.h>
#include <map>
#include <algorithm>
#include <set>

#include "../include/lock_master.h"
#include "../include/nodegit.h"
#include "../include/wrapper.h"
#include "../include/promise_completion.h"
#include "../include/functions/copy.h"
{% each %}
  {% if type != "enum" %}
    #include "../include/{{ filename }}.h"
  {% endif %}
{% endeach %}
#include "../include/convenient_patch.h"
#include "../include/convenient_hunk.h"

void LockMasterEnable(const FunctionCallbackInfo<Value>& info) {
  LockMaster::Enable();
}

void LockMasterDisable(const FunctionCallbackInfo<Value>& info) {
  LockMaster::Disable();
}

void LockMasterIsEnabled(const FunctionCallbackInfo<Value>& info) {
  info.GetReturnValue().Set(Nan::New(LockMaster::IsEnabled()));
}

void LockMasterGetDiagnostics(const FunctionCallbackInfo<Value>& info) {
  LockMaster::Diagnostics diagnostics(LockMaster::GetDiagnostics());

  // return a plain JS object with properties
  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  result->Set(Nan::New("storedMutexesCount").ToLocalChecked(), Nan::New(diagnostics.storedMutexesCount));
  info.GetReturnValue().Set(result);
}

void run_libgit2_event_queue(void *);

extern "C" void init(Local<v8::Object> target) {
  // Initialize libgit2.
  git_libgit2_init();

  Nan::HandleScope scope;

  Wrapper::InitializeComponent(target);
  PromiseCompletion::InitializeComponent();
  {% each %}
    {% if type != "enum" %}
      {{ cppClassName }}::InitializeComponent(target);
    {% endif %}
  {% endeach %}

  // initialize a thread on which we will execute all libgit2 calls
  // for async NodeGit wrappers (and sync ones, eventually)
  libgit2_loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
  uv_loop_init(libgit2_loop);

  uv_thread_t libgit2_thread_id;
  uv_thread_create(&libgit2_thread_id, run_libgit2_event_queue, NULL);
  
  ConvenientHunk::InitializeComponent(target);
  ConvenientPatch::InitializeComponent(target);

  NODE_SET_METHOD(target, "enableThreadSafety", LockMasterEnable);
  NODE_SET_METHOD(target, "disableThreadSafety", LockMasterDisable);
  NODE_SET_METHOD(target, "isThreadSafetyEnabled", LockMasterIsEnabled);
  NODE_SET_METHOD(target, "getThreadSafetyDiagnostics", LockMasterGetDiagnostics);

  LockMaster::Initialize();
}

NODE_MODULE(nodegit, init)

uv_loop_t *libgit2_loop;

void run_libgit2_event_queue(void *)
{
  // run, CPU, run
  for ( ; ; ) {
    uv_run(libgit2_loop, UV_RUN_DEFAULT);
    // TODO: break out of loop
  }

  uv_loop_close(libgit2_loop);
  free(libgit2_loop);
}
