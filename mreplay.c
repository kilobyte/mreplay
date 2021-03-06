#include <fcntl.h>
#include <malloc.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static void die(const char *fmt, ...)
{
    char buf[512];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    write(2, buf, len);
    exit(1);
}

#define MAXTH 4096

#define NSLOTS 1048576
static void *slots[NSLOTS];

struct rentry
{
    uint32_t func;
    uint32_t slot;
    uint32_t size;
    uint32_t arg2;
};

static struct rentry *m;
static size_t blen;
static unsigned nthreads = 0;

static void* worker(void* arg)
{
    unsigned nth = nthreads;
    unsigned tid = (uintptr_t)arg;
    size_t len = blen / sizeof(struct rentry);
    struct rentry *end = m + len;

    for (struct rentry *r = m; r < end; r++)
    {
        void *m;
        uint32_t slot = r->slot;
        if (slot >= NSLOTS)
            die("Slot out of range: %u\n", slot);
        if (slot % nth != tid)
            continue;

        switch (r->func)
        {
        case 1:
            m = malloc(r->size);
            if (!m)
                die("malloc(%u) failed: %m\n", r->size);
            slots[slot] = m;
            break;
        case 2:
            if (!slots[slot] != !slot)
                die("free() without a malloc().\n");
            free(slots[slot]);
            slots[slot] = NULL;
            break;
        case 3:
            if (!slots[slot])
                die("realloc() without a malloc().\n");
            m = realloc(slots[slot], r->size);
            if (!m)
                die("realloc(%d) failed: %m\n", r->size);
            slots[slot] = m;
            break;
        case 4:
            m = realloc(NULL, r->size);
            if (!m)
                die("realloc(0 -> %u) failed: %m\n", r->size);
            slots[slot] = m;
            break;
        case 5:
            m = calloc(r->size, r->arg2);
            if (!m)
                die("calloc(%u, %u) failed: %m\n", r->size, r->arg2);
            slots[slot] = m;
            break;
        case 6:
            m = memalign(r->arg2, r->size);
            if (!m)
                die("memalign(%u, %u) failed: %m\n", r->arg2, r->size);
            slots[slot] = m;
            break;
        default:
            die("Unknown function %u\n", r->func);
        }
    }

    return 0;
}

int main(int argc, const char **argv)
{
    if (!argv[1])
        die("Usage: mreplay <file.mr> [#threads]\n");

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
        die("open(%s): %m\n", argv[1]);

    struct stat st;
    if (fstat(fd, &st))
        die("stat(%s): %m\n", argv[1]);
    blen = st.st_size;
    if (!blen)
        die("Can't replay a zero-size file.\n");
    m = mmap(0, blen, PROT_READ, MAP_SHARED, fd, 0);
    if (m == MAP_FAILED)
        die("mmap(%s): %m\n", argv[1]);
    close(fd);
    madvise(m, blen, MADV_SEQUENTIAL);
    slots[0] = NULL;

    if (argv[2])
        nthreads = atoi(argv[2]);
    if (nthreads > MAXTH)
        die("Too many threads, max is %u\n", MAXTH);

    if (!nthreads)
    {
        nthreads = 1;
        worker(0);
        return 0;
    }

    pthread_t th[MAXTH];
    for (unsigned i = 0; i < nthreads; i++)
    {
        if (pthread_create(&th[i], 0, worker, (void*)(uintptr_t)i))
            die("pthread_create(%u) failed: %m\n", i);
    }

    // threads do stuff here

    for (unsigned i = 0; i < nthreads; i++)
    {
        if (pthread_join(th[i], 0))
            die("pthread_join(%u) failed: %m\n", i);
    }

    return 0;
}
