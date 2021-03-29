#include <stdlib.h>

int main()
{
    char *m = malloc(123);
    *m = 1;
    free(m);

    m = malloc(4096);
    *m = 1;
    free(m);

    m = malloc(256 * 1048576);
    *m = 1;
    free(m);

    return 0;
}

