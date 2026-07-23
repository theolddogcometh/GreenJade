/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * mallinfo / mallinfo2 / mallopt / malloc_stats / malloc_info surface.
 * Soft deepen: report freelist/arena figures from the freestanding heap.
 */
#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Defined in malloc.c — soft heap accounting. */
void _libcgj_heap_query(size_t *pcbArena, size_t *pcbUord, size_t *pcbFord,
                        size_t *pcOrdblks);

static int
clamp_int(size_t n)
{
    if (n > (size_t)0x7fffffff) {
        return 0x7fffffff;
    }
    return (int)n;
}

struct mallinfo
mallinfo(void)
{
    struct mallinfo m;
    size_t cbArena = 0;
    size_t cbUord = 0;
    size_t cbFord = 0;
    size_t cOrd = 0;

    memset(&m, 0, sizeof(m));
    _libcgj_heap_query(&cbArena, &cbUord, &cbFord, &cOrd);
    m.arena = clamp_int(cbArena);
    m.ordblks = clamp_int(cOrd);
    m.uordblks = clamp_int(cbUord);
    m.fordblks = clamp_int(cbFord);
    m.keepcost = clamp_int(cbFord); /* soft: all free is trimmable in theory */
    return m;
}

struct mallinfo2
mallinfo2(void)
{
    struct mallinfo2 m;
    size_t cbArena = 0;
    size_t cbUord = 0;
    size_t cbFord = 0;
    size_t cOrd = 0;

    memset(&m, 0, sizeof(m));
    _libcgj_heap_query(&cbArena, &cbUord, &cbFord, &cOrd);
    m.arena = cbArena;
    m.ordblks = cOrd;
    m.uordblks = cbUord;
    m.fordblks = cbFord;
    m.keepcost = cbFord;
    return m;
}

int
mallopt(int nParam, int nVal)
{
    (void)nParam;
    (void)nVal;
    /* Accepted soft knobs; freestanding heap has no tunable arenas yet. */
    return 1;
}

void
malloc_stats(void)
{
    size_t cbArena = 0;
    size_t cbUord = 0;
    size_t cbFord = 0;
    size_t cOrd = 0;
    char aBuf[192];
    int n;
    size_t i;
    size_t cbOut;

    _libcgj_heap_query(&cbArena, &cbUord, &cbFord, &cOrd);
    /* Avoid printf dependency on stderr path depth; format by hand. */
    n = snprintf(aBuf, sizeof(aBuf),
                 "libcgj malloc_stats: arena=%zu uord=%zu ford=%zu ordblks=%zu\n",
                 cbArena, cbUord, cbFord, cOrd);
    if (n < 0) {
        return;
    }
    cbOut = (size_t)n;
    if (cbOut >= sizeof(aBuf)) {
        cbOut = sizeof(aBuf) - 1;
    }
    i = 0;
    while (i < cbOut) {
        ssize_t w = write(2, aBuf + i, cbOut - i);

        if (w <= 0) {
            break;
        }
        i += (size_t)w;
    }
}

int
malloc_info(int nOptions, FILE *pF)
{
    size_t cbArena = 0;
    size_t cbUord = 0;
    size_t cbFord = 0;
    size_t cOrd = 0;

    (void)nOptions;
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    _libcgj_heap_query(&cbArena, &cbUord, &cbFord, &cOrd);
    if (fprintf(pF,
                "<malloc version=\"libcgj-1\">\n"
                "  <heap nr=\"0\">\n"
                "    <sizes>\n"
                "      <size from=\"0\" to=\"%zu\" total=\"%zu\" count=\"%zu\"/>\n"
                "    </sizes>\n"
                "    <total type=\"fast\" count=\"0\" size=\"0\"/>\n"
                "    <total type=\"rest\" count=\"%zu\" size=\"%zu\"/>\n"
                "    <system type=\"current\" size=\"%zu\"/>\n"
                "    <system type=\"max\" size=\"%zu\"/>\n"
                "    <aspace type=\"total\" size=\"%zu\"/>\n"
                "    <aspace type=\"mprotect\" size=\"%zu\"/>\n"
                "  </heap>\n"
                "  <total type=\"fast\" count=\"0\" size=\"0\"/>\n"
                "  <total type=\"rest\" count=\"%zu\" size=\"%zu\"/>\n"
                "  <system type=\"current\" size=\"%zu\"/>\n"
                "  <system type=\"max\" size=\"%zu\"/>\n"
                "  <aspace type=\"total\" size=\"%zu\"/>\n"
                "  <aspace type=\"mprotect\" size=\"%zu\"/>\n"
                "</malloc>\n",
                cbFord, cbFord, cOrd,
                cOrd, cbFord,
                cbArena, cbArena, cbArena, cbArena,
                cOrd, cbUord,
                cbArena, cbArena, cbArena, cbArena) < 0) {
        return -1;
    }
    return 0;
}
