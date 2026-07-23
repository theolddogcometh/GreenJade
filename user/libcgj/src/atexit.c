/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * atexit / __cxa_atexit / on_exit handlers for exit().
 */
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define CGJ_ATEXIT_MAX 32

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
static int g_nSlots;
static int g_fRunning;

void
_libcgj_run_atexit(int nCode)
{
    int i;

    if (g_fRunning) {
        return;
    }
    g_fRunning = 1;
    for (i = g_nSlots - 1; i >= 0; i--) {
        if (g_aSlots[i].nKind == 1 && g_aSlots[i].pfn != NULL) {
            g_aSlots[i].pfn();
        } else if (g_aSlots[i].nKind == 2 && g_aSlots[i].pfnCxa != NULL) {
            g_aSlots[i].pfnCxa(g_aSlots[i].pArg);
        } else if (g_aSlots[i].nKind == 3 && g_aSlots[i].pfnOn != NULL) {
            g_aSlots[i].pfnOn(nCode, g_aSlots[i].pArg);
        }
        g_aSlots[i].nKind = 0;
    }
    g_nSlots = 0;
    g_fRunning = 0;
}

int
atexit(void (*pfn)(void))
{
    if (pfn == NULL || g_nSlots >= CGJ_ATEXIT_MAX) {
        return -1;
    }
    g_aSlots[g_nSlots].nKind = 1;
    g_aSlots[g_nSlots].pfn = pfn;
    g_aSlots[g_nSlots].pfnCxa = NULL;
    g_aSlots[g_nSlots].pfnOn = NULL;
    g_aSlots[g_nSlots].pArg = NULL;
    g_aSlots[g_nSlots].pDso = NULL;
    g_nSlots++;
    return 0;
}

int
on_exit(void (*pfn)(int, void *), void *pArg)
{
    if (pfn == NULL || g_nSlots >= CGJ_ATEXIT_MAX) {
        return -1;
    }
    g_aSlots[g_nSlots].nKind = 3;
    g_aSlots[g_nSlots].pfn = NULL;
    g_aSlots[g_nSlots].pfnCxa = NULL;
    g_aSlots[g_nSlots].pfnOn = pfn;
    g_aSlots[g_nSlots].pArg = pArg;
    g_aSlots[g_nSlots].pDso = NULL;
    g_nSlots++;
    return 0;
}

int
__cxa_atexit(void (*pfn)(void *), void *pArg, void *pDso)
{
    if (pfn == NULL || g_nSlots >= CGJ_ATEXIT_MAX) {
        return -1;
    }
    g_aSlots[g_nSlots].nKind = 2;
    g_aSlots[g_nSlots].pfn = NULL;
    g_aSlots[g_nSlots].pfnCxa = (cxa_fn)(uintptr_t)pfn;
    g_aSlots[g_nSlots].pfnOn = NULL;
    g_aSlots[g_nSlots].pArg = pArg;
    g_aSlots[g_nSlots].pDso = pDso;
    g_nSlots++;
    return 0;
}

void
__cxa_finalize(void *pDso)
{
    int i;

    if (pDso == NULL) {
        _libcgj_run_atexit(0);
        return;
    }
    for (i = g_nSlots - 1; i >= 0; i--) {
        if (g_aSlots[i].nKind == 2 && g_aSlots[i].pDso == pDso) {
            if (g_aSlots[i].pfnCxa != NULL) {
                g_aSlots[i].pfnCxa(g_aSlots[i].pArg);
            }
            g_aSlots[i].nKind = 0;
        }
    }
}
