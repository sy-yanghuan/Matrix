/*
 * stacktrace.cpp
 *
 *  Created on: 2012-9-6
 *      Author: YH
 */

#include "stacktrace.hpp"

#include <unwind.h> // ABI defined unwinder
#include <matrix/base/debug.hpp>

namespace matrix {
namespace base {

typedef struct {
  void **result;
  int max_depth;
  int skip_count;
  int count;
} trace_arg_t;

// Workaround for the malloc() in _Unwind_Backtrace() issue.
static _Unwind_Reason_Code nop_backtrace(struct _Unwind_Context *uc, void *opq) {
  return _URC_NO_REASON;
}


// This code is not considered ready to run until
// static initializers run so that we are guaranteed
// that any malloc-related initialization is done.
static bool ready_to_run = false;
class StackTraceInit {
 public:
   StackTraceInit() {
     // Extra call to force initialization
     _Unwind_Backtrace(nop_backtrace, nullptr);
     ready_to_run = true;
   }
};

static StackTraceInit module_initializer;  // Force initialization

static _Unwind_Reason_Code getOneFrame(struct _Unwind_Context *uc, void *opq) {
  trace_arg_t *targ = (trace_arg_t *) opq;

  if (targ->skip_count > 0) {
    targ->skip_count--;
  } else {
    targ->result[targ->count] = (void *) _Unwind_GetIP(uc);

    if (targ->count > 0 && targ->result[targ->count - 1] == targ->result[targ->count])
     return _URC_END_OF_STACK;
  }

  if (++targ->count == targ->max_depth)
    return _URC_END_OF_STACK;

  return _URC_NO_REASON;
}

int stackTrace(void** result, int max_depth, int skip_count) {

    if (!ready_to_run)
      return 0;

    trace_arg_t targ;

    skip_count += 1;         // Do not include the "GetStackTrace" frame

    targ.result = result;
    targ.max_depth = max_depth;
    targ.skip_count = skip_count;
    targ.count = 0;

    _Unwind_Backtrace(getOneFrame, &targ);

    return targ.count;
}

char* (*stacktrace_)(char*, size_t, const void*) = nullptr;

char* stacktrace(char* str, size_t size, const void* context) {
    ASSERT(stacktrace_ != nullptr);
    return stacktrace_(str, size , context);
}

} /* namespace base */
} /* namespace matrix */
