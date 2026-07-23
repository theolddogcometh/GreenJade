/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * pthread_atfork / __register_atfork — fork prepare/parent/child hooks.
 * Not GNU glibc (public ABI surface only).
 *
 * greppable: CGJ_ATFORK_SOFT_SLOT_REUSE
 * greppable: CGJ_ATFORK_SOFT_LIFO_PREPARE
 * greppable: CGJ_ATFORK_SOFT_DSO_UNREG
 *
 * Soft deepen: reclaim unused slots, LIFO prepare / FIFO parent+child,
 * soft DSO unregister for dlclose-shaped cleanup. Called from fork() in
 * syscall_linux.c via _libcgj_atfork_*.
 */
#include <errno.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>

#define CGJ_ATFORK_MAX 32

struct atfork_slot {
    void (*pfnPrepare)(void);
    void (*pfnParent)(void);
    void (*pfnChild)(void);
    void  *pDso;
    int    fUsed;
};

static struct atfork_slot g_aAtfork[CGJ_ATFORK_MAX];
static int g_nAtfork; /* high-water past last used index */

static int
atfork_soft_alloc_slot(void)
{
    int iSlot;

    /* greppable: CGJ_ATFORK_SOFT_SLOT_REUSE */
    for (iSlot = 0; iSlot < g_nAtfork; iSlot++) {
        if (!g_aAtfork[iSlot].fUsed) {
            return iSlot;
        }
    }
    if (g_nAtfork >= CGJ_ATFORK_MAX) {
        return -1;
    }
    iSlot = g_nAtfork;
    g_nAtfork++;
    return iSlot;
}

int
__register_atfork(void (*pfnPrepare)(void), void (*pfnParent)(void),
                  void (*pfnChild)(void), void *pDso)
{
    int iSlot;

    iSlot = atfork_soft_alloc_slot();
    if (iSlot < 0) {
        errno = ENOMEM;
        return -1;
    }
    g_aAtfork[iSlot].pfnPrepare = pfnPrepare;
    g_aAtfork[iSlot].pfnParent = pfnParent;
    g_aAtfork[iSlot].pfnChild = pfnChild;
    g_aAtfork[iSlot].pDso = pDso;
    g_aAtfork[iSlot].fUsed = 1;
    return 0;
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

/*
 * Soft DSO unregister (dlclose-shaped). Safe no-op if pDso is NULL.
 * greppable: CGJ_ATFORK_SOFT_DSO_UNREG
 */
void
_libcgj_atfork_unregister_dso(void *pDso)
{
    int iSlot;

    if (pDso == NULL) {
        return;
    }
    for (iSlot = 0; iSlot < g_nAtfork; iSlot++) {
        if (g_aAtfork[iSlot].fUsed && g_aAtfork[iSlot].pDso == pDso) {
            g_aAtfork[iSlot].fUsed = 0;
            g_aAtfork[iSlot].pfnPrepare = NULL;
            g_aAtfork[iSlot].pfnParent = NULL;
            g_aAtfork[iSlot].pfnChild = NULL;
            g_aAtfork[iSlot].pDso = NULL;
        }
    }
}

/* Called from fork() in syscall_linux.c */
void
_libcgj_atfork_prepare(void)
{
    int iSlot;

    /* greppable: CGJ_ATFORK_SOFT_LIFO_PREPARE */
    for (iSlot = g_nAtfork - 1; iSlot >= 0; iSlot--) {
        if (g_aAtfork[iSlot].fUsed && g_aAtfork[iSlot].pfnPrepare != NULL) {
            g_aAtfork[iSlot].pfnPrepare();
        }
    }
}

void
_libcgj_atfork_parent(void)
{
    int iSlot;

    for (iSlot = 0; iSlot < g_nAtfork; iSlot++) {
        if (g_aAtfork[iSlot].fUsed && g_aAtfork[iSlot].pfnParent != NULL) {
            g_aAtfork[iSlot].pfnParent();
        }
    }
}

void
_libcgj_atfork_child(void)
{
    int iSlot;

    for (iSlot = 0; iSlot < g_nAtfork; iSlot++) {
        if (g_aAtfork[iSlot].fUsed && g_aAtfork[iSlot].pfnChild != NULL) {
            g_aAtfork[iSlot].pfnChild();
        }
    }
}
