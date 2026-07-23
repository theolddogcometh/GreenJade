/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * atexit / __cxa_atexit / on_exit handlers for exit().
 *
 * greppable: CGJ_ATEXIT_SOFT_SLOT_REUSE
 * greppable: CGJ_ATEXIT_SOFT_LIFO
 * greppable: CGJ_CXA_FINALIZE_SOFT_DSO
 *
 * Soft deepen: reclaim empty slots after __cxa_finalize, LIFO run on exit,
 * slightly larger soft table. __cxa_thread_atexit* live in graph batches.
 */
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define CGJ_ATEXIT_MAX 64

typedef void (*cxa_fn)(void *);

struct atexit_slot {
    int     nKind; /* 0=empty 1=atexit 2=cxa 3=on_exit */
    void  (*pfn)(void);
    cxa_fn  pfnCxa;
    void  (*pfnOn)(int, void *);
    void   *pArg;
    void   *pDso;
};

static struct atexit_slot g_aSlots[CGJ_ATEXIT_MAX];
static int g_nSlots; /* high-water: first free index past last used */
static int g_fRunning;

static int
atexit_soft_alloc_slot(void)
{
    int iSlot;

    /* greppable: CGJ_ATEXIT_SOFT_SLOT_REUSE */
    for (iSlot = 0; iSlot < g_nSlots; iSlot++) {
        if (g_aSlots[iSlot].nKind == 0) {
            return iSlot;
        }
    }
    if (g_nSlots >= CGJ_ATEXIT_MAX) {
        return -1;
    }
    iSlot = g_nSlots;
    g_nSlots++;
    return iSlot;
}

static void
atexit_soft_clear_slot(int iSlot)
{
    if (iSlot < 0 || iSlot >= CGJ_ATEXIT_MAX) {
        return;
    }
    g_aSlots[iSlot].nKind = 0;
    g_aSlots[iSlot].pfn = NULL;
    g_aSlots[iSlot].pfnCxa = NULL;
    g_aSlots[iSlot].pfnOn = NULL;
    g_aSlots[iSlot].pArg = NULL;
    g_aSlots[iSlot].pDso = NULL;
}

void
_libcgj_run_atexit(int nCode)
{
    int iSlot;

    if (g_fRunning) {
        return;
    }
    g_fRunning = 1;
    /* greppable: CGJ_ATEXIT_SOFT_LIFO */
    for (iSlot = g_nSlots - 1; iSlot >= 0; iSlot--) {
        int nKind = g_aSlots[iSlot].nKind;

        if (nKind == 1 && g_aSlots[iSlot].pfn != NULL) {
            void (*pfn)(void) = g_aSlots[iSlot].pfn;

            atexit_soft_clear_slot(iSlot);
            pfn();
        } else if (nKind == 2 && g_aSlots[iSlot].pfnCxa != NULL) {
            cxa_fn pfnCxa = g_aSlots[iSlot].pfnCxa;
            void  *pArg = g_aSlots[iSlot].pArg;

            atexit_soft_clear_slot(iSlot);
            pfnCxa(pArg);
        } else if (nKind == 3 && g_aSlots[iSlot].pfnOn != NULL) {
            void (*pfnOn)(int, void *) = g_aSlots[iSlot].pfnOn;
            void *pArg = g_aSlots[iSlot].pArg;

            atexit_soft_clear_slot(iSlot);
            pfnOn(nCode, pArg);
        } else {
            atexit_soft_clear_slot(iSlot);
        }
    }
    g_nSlots = 0;
    g_fRunning = 0;
}

int
atexit(void (*pfn)(void))
{
    int iSlot;

    if (pfn == NULL) {
        return -1;
    }
    iSlot = atexit_soft_alloc_slot();
    if (iSlot < 0) {
        return -1;
    }
    g_aSlots[iSlot].nKind = 1;
    g_aSlots[iSlot].pfn = pfn;
    g_aSlots[iSlot].pfnCxa = NULL;
    g_aSlots[iSlot].pfnOn = NULL;
    g_aSlots[iSlot].pArg = NULL;
    g_aSlots[iSlot].pDso = NULL;
    return 0;
}

int
on_exit(void (*pfn)(int, void *), void *pArg)
{
    int iSlot;

    if (pfn == NULL) {
        return -1;
    }
    iSlot = atexit_soft_alloc_slot();
    if (iSlot < 0) {
        return -1;
    }
    g_aSlots[iSlot].nKind = 3;
    g_aSlots[iSlot].pfn = NULL;
    g_aSlots[iSlot].pfnCxa = NULL;
    g_aSlots[iSlot].pfnOn = pfn;
    g_aSlots[iSlot].pArg = pArg;
    g_aSlots[iSlot].pDso = NULL;
    return 0;
}

int
__cxa_atexit(void (*pfn)(void *), void *pArg, void *pDso)
{
    int iSlot;

    if (pfn == NULL) {
        return -1;
    }
    iSlot = atexit_soft_alloc_slot();
    if (iSlot < 0) {
        return -1;
    }
    g_aSlots[iSlot].nKind = 2;
    g_aSlots[iSlot].pfn = NULL;
    g_aSlots[iSlot].pfnCxa = (cxa_fn)(uintptr_t)pfn;
    g_aSlots[iSlot].pfnOn = NULL;
    g_aSlots[iSlot].pArg = pArg;
    g_aSlots[iSlot].pDso = pDso;
    return 0;
}

void
__cxa_finalize(void *pDso)
{
    int iSlot;

    /* greppable: CGJ_CXA_FINALIZE_SOFT_DSO */
    if (pDso == NULL) {
        _libcgj_run_atexit(0);
        return;
    }
    /* LIFO: only matching DSO cxa handlers; leave others for exit. */
    for (iSlot = g_nSlots - 1; iSlot >= 0; iSlot--) {
        if (g_aSlots[iSlot].nKind == 2 && g_aSlots[iSlot].pDso == pDso) {
            cxa_fn pfnCxa = g_aSlots[iSlot].pfnCxa;
            void  *pArg = g_aSlots[iSlot].pArg;

            atexit_soft_clear_slot(iSlot);
            if (pfnCxa != NULL) {
                pfnCxa(pArg);
            }
        }
    }
}
