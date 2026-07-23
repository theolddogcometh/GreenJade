/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * pthread_atfork / __register_atfork — fork prepare/parent/child hooks.
 * Not GNU glibc (public ABI surface only).
 */
#include <errno.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>

#define CGJ_ATFORK_MAX 16

struct atfork_slot {
    void (*pfnPrepare)(void);
    void (*pfnParent)(void);
    void (*pfnChild)(void);
    void  *pDso;
    int    fUsed;
};

static struct atfork_slot g_aAtfork[CGJ_ATFORK_MAX];
static int g_nAtfork;

int
__register_atfork(void (*pfnPrepare)(void), void (*pfnParent)(void),
                  void (*pfnChild)(void), void *pDso)
{
    int i;

    for (i = 0; i < CGJ_ATFORK_MAX; i++) {
        if (!g_aAtfork[i].fUsed) {
            g_aAtfork[i].pfnPrepare = pfnPrepare;
            g_aAtfork[i].pfnParent = pfnParent;
            g_aAtfork[i].pfnChild = pfnChild;
            g_aAtfork[i].pDso = pDso;
            g_aAtfork[i].fUsed = 1;
            if (i >= g_nAtfork) {
                g_nAtfork = i + 1;
            }
            return 0;
        }
    }
    errno = ENOMEM;
    return -1;
}

int
pthread_atfork(void (*pfnPrepare)(void), void (*pfnParent)(void),
               void (*pfnChild)(void))
{
    if (__register_atfork(pfnPrepare, pfnParent, pfnChild, NULL) != 0) {
        return errno != 0 ? errno : ENOMEM;
    }
    return 0;
}

/* Called from fork() in syscall_linux.c */
void
_libcgj_atfork_prepare(void)
{
    int i;

    /* LIFO prepare (glibc-shaped) */
    for (i = g_nAtfork - 1; i >= 0; i--) {
        if (g_aAtfork[i].fUsed && g_aAtfork[i].pfnPrepare != NULL) {
            g_aAtfork[i].pfnPrepare();
        }
    }
}

void
_libcgj_atfork_parent(void)
{
    int i;

    for (i = 0; i < g_nAtfork; i++) {
        if (g_aAtfork[i].fUsed && g_aAtfork[i].pfnParent != NULL) {
            g_aAtfork[i].pfnParent();
        }
    }
}

void
_libcgj_atfork_child(void)
{
    int i;

    for (i = 0; i < g_nAtfork; i++) {
        if (g_aAtfork[i].fUsed && g_aAtfork[i].pfnChild != NULL) {
            g_aAtfork[i].pfnChild();
        }
    }
}
