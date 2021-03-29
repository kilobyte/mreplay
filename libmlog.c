// SPDX-License-Identifier: BSD-2-Clause
/* Copyright (C) 2021 Intel Corporation. */
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

#define MEMTIER_EXPORT __attribute__((visibility("default")))

// glibc defines these, a version portable to others libcs would need to call
// dlsym() at runtime.
extern void *__libc_malloc(size_t size);
extern void *__libc_calloc(size_t nmemb, size_t size);
extern void *__libc_realloc(void *ptr, size_t size);
extern void *__libc_memalign(size_t boundary, size_t size);
extern void __libc_free(void *ptr);

static void log_debug(const char *fmt, ...)
{
    char buf[512];

    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    write(1, buf, len);
}

MEMTIER_EXPORT void *malloc(size_t size)
{
    void *ret = __libc_malloc(size);
    log_debug("malloc %zu = %p\n", size, ret);
    return ret;
}

MEMTIER_EXPORT void *calloc(size_t nmemb, size_t size)
{
    void *ret = __libc_calloc(nmemb, size);
    log_debug("calloc %zu %zu = %p\n", nmemb, size, ret);
    return ret;
}

MEMTIER_EXPORT void *realloc(void *ptr, size_t size)
{
    void *ret = __libc_realloc(ptr, size);
    log_debug("realloc %p %zu = %p\n", ptr, size, ret);
    return ret;
}

MEMTIER_EXPORT void *memalign(size_t boundary, size_t size)
{
    void *ret = __libc_memalign(boundary, size);
    log_debug("memalign %zu %zu = %p\n", boundary, size, ret);
    return ret;
}

MEMTIER_EXPORT void free(void *ptr)
{
    log_debug("free %p\n", ptr);
    return __libc_free(ptr);
}
