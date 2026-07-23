/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * mallinfo / mallinfo2 / mallopt / malloc_stats / malloc_info surface.
 * Bring-up: zeros + ENOSYS-style soft stats (no internal heap walk yet).
 */
#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

struct mallinfo
mallinfo(void)
{
    struct mallinfo m;

    memset(&m, 0, sizeof(m));
    return m;
}

struct mallinfo2
mallinfo2(void)
{
    struct mallinfo2 m;

    memset(&m, 0, sizeof(m));
    return m;
}

int
mallopt(int nParam, int nVal)
{
    (void)nParam;
    (void)nVal;
    return 1; /* success no-op */
}

void
malloc_stats(void)
{
    const char sz[] = "libcgj malloc_stats: bring-up (no detail)\n";

    (void)write(2, sz, sizeof(sz) - 1);
}

int
malloc_info(int nOptions, FILE *pF)
{
    (void)nOptions;
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    (void)fputs("<malloc version=\"libcgj-bringup\"/>\n", pF);
    return 0;
}
