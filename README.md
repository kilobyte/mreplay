Tools here:
===========

* *simple* — a minimal test executable that calls `malloc()`/`free()`

* libmlog — a tool to intercept and log `malloc` calls.  Usage:
  `LD_PRELOAD=./libmlog.so my-grandiose-task`

* ml2mr — convert the above log to a binary form
  (`mreplay` wants the latter)

* mreplay — replay a binary log


ml2mr
=====

Usage: `ml2mr <log.ml >log.mr`

Analyzes the captured log, assigns reusable but non-overlaping "slots" to
particular allocations, so mreplay won't have to "think" during a run.


mreplay
=======

Usage: `mreplay log.mr [#threads]`

When ran with no second argument, issued malloc-like calls will exactly
match those captured by the log.

In threaded mode, there's at least one extra alloc per thread, and the order
of allocs is not preserved.  There's currently no attempt to equalize each
thread's workload except for a rough initial heurestic.
