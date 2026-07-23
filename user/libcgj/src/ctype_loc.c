/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * __ctype_b_loc / __ctype_tolower_loc / __ctype_toupper_loc (glibc graph).
 * ASCII C locale tables for apps that dig into ctype internals.
 *
 * greppable: CGJ_CTYPE_LOC_SOFT
 * greppable: CGJ_CTYPE_LOC_SOFT_ONCE
 *
 * Soft deepen: one-shot init of 384-slot tables (EOF-safe index via +128)
 * and consistent _ISbit layout for C locale diggers.
 */
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>

/* glibc _ISbit layout (network byte order bits on big-endian; LE host uses
 * the documented bit values from ctype.h public ABI). */
#define _ISupper  0x0001
#define _ISlower  0x0002
#define _ISalpha  0x0004
#define _ISdigit  0x0008
#define _ISxdigit 0x0010
#define _ISspace  0x0020
#define _ISprint  0x0040
#define _ISgraph  0x0080
#define _ISblank  0x0100
#define _IScntrl  0x0200
#define _ISpunct  0x0400
#define _ISalnum  0x0800

#define CGJ_CTYPE_LOC_SLOTS 384
#define CGJ_CTYPE_LOC_BASE  128

static unsigned short g_aCtypeB[CGJ_CTYPE_LOC_SLOTS];
static int           g_aTolower[CGJ_CTYPE_LOC_SLOTS];
static int           g_aToupper[CGJ_CTYPE_LOC_SLOTS];
static int           g_fInit;

static void
ctype_loc_init(void)
{
    int iSlot;

    /* greppable: CGJ_CTYPE_LOC_SOFT_ONCE */
    if (g_fInit) {
        return;
    }
    for (iSlot = 0; iSlot < CGJ_CTYPE_LOC_SLOTS; iSlot++) {
        int            nC = iSlot - CGJ_CTYPE_LOC_BASE;
        unsigned short uBits = 0;

        g_aTolower[iSlot] = nC;
        g_aToupper[iSlot] = nC;
        if (nC >= 0 && nC < 256) {
            if (nC >= 'A' && nC <= 'Z') {
                uBits |= (unsigned short)(_ISupper | _ISalpha | _ISalnum |
                                          _ISgraph | _ISprint);
                g_aTolower[iSlot] = nC + ('a' - 'A');
            }
            if (nC >= 'a' && nC <= 'z') {
                uBits |= (unsigned short)(_ISlower | _ISalpha | _ISalnum |
                                          _ISgraph | _ISprint);
                g_aToupper[iSlot] = nC - ('a' - 'A');
            }
            if (nC >= '0' && nC <= '9') {
                uBits |= (unsigned short)(_ISdigit | _ISxdigit | _ISalnum |
                                          _ISgraph | _ISprint);
            }
            if ((nC >= 'a' && nC <= 'f') || (nC >= 'A' && nC <= 'F')) {
                uBits |= (unsigned short)_ISxdigit;
            }
            if (nC == ' ' || nC == '\t' || nC == '\n' || nC == '\r' ||
                nC == '\f' || nC == '\v') {
                uBits |= (unsigned short)_ISspace;
            }
            if (nC == ' ' || nC == '\t') {
                uBits |= (unsigned short)_ISblank;
            }
            if (nC >= 0x20 && nC <= 0x7e) {
                uBits |= (unsigned short)_ISprint;
            }
            if ((nC >= 0 && nC < 0x20) || nC == 0x7f) {
                uBits |= (unsigned short)_IScntrl;
            }
            /* punct: printable, not space, not alnum */
            if ((uBits & _ISprint) && !(uBits & _ISspace) &&
                !(uBits & _ISalnum)) {
                uBits |= (unsigned short)(_ISpunct | _ISgraph);
            }
            /* graph: printable non-space */
            if ((uBits & _ISprint) && !(uBits & _ISspace)) {
                uBits |= (unsigned short)_ISgraph;
            }
        }
        g_aCtypeB[iSlot] = uBits;
    }
    g_fInit = 1;
}

const unsigned short **
__ctype_b_loc(void)
{
    static const unsigned short *pTab;

    ctype_loc_init();
    pTab = &g_aCtypeB[CGJ_CTYPE_LOC_BASE];
    return &pTab;
}

const int **
__ctype_tolower_loc(void)
{
    static const int *pTab;

    ctype_loc_init();
    pTab = &g_aTolower[CGJ_CTYPE_LOC_BASE];
    return &pTab;
}

const int **
__ctype_toupper_loc(void)
{
    static const int *pTab;

    ctype_loc_init();
    pTab = &g_aToupper[CGJ_CTYPE_LOC_BASE];
    return &pTab;
}

/* glibc also exports __ctype_get_mb_cur_max for locale. */
size_t
__ctype_get_mb_cur_max(void)
{
    return 1; /* C locale soft: single-byte */
}
