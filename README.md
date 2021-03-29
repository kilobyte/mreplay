Tools here:
===========

* *simple* — a minimal test executable that calls `malloc()`/`free()`

* libmlog — a tool to intercept and log `malloc` calls.  Usage:
  `LD_PRELOAD=./libmlog.so my-grandiose-task`

* ml2mr — convert the above log to a binary form
  (`mreplay` wants the latter)

* mreplay — replay a binary log
