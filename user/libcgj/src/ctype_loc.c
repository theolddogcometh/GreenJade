/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * __ctype_b_loc / __ctype_tolower_loc / __ctype_toupper_loc (glibc graph).
 * ASCII C locale tables for apps that dig into ctype internals.
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

static unsigned short g_aCtypeB[384];
static int g_aTolower[384];
static int g_aToupper[384];
static int g_fInit;

static void
ctype_loc_init(void)
{
    int i;

    if (g_fInit) {
        return;
    }
    for (i = 0; i < 384; i++) {
        int c = i - 128;
        unsigned short b = 0;

        g_aTolower[i] = c;
        g_aToupper[i] = c;
        if (c >= 0 && c < 256) {
            if (c >= 'A' && c <= 'Z') {
                b |= _ISupper | _ISalpha | _ISalnum | _ISgraph | _ISprint;
                g_aTolower[i] = c + ('a' - 'A');
            }
            if (c >= 'a' && c <= 'z') {
                b |= _ISlower | _ISalpha | _ISalnum | _ISgraph | _ISprint;
                g_aToupper[i] = c - ('a' - 'A');
            }
            if (c >= '0' && c <= '9') {
                b |= _ISdigit | _ISxdigit | _ISalnum | _ISgraph | _ISprint;
            }
            if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                b |= _ISxdigit;
            }
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' ||
                c == '\v') {
                b |= _ISspace;
            }
            if (c == ' ' || c == '\t') {
                b |= _ISblank;
            }
            if (c >= 0x20 && c <= 0x7e) {
                b |= _ISprint;
            }
            if ((c >= 0 && c < 0x20) || c == 0x7f) {
                b |= _IScntrl;
            }
            if ((b & _ISprint) && !(b & _ISspace) && !(b & _ISalnum)) {
                b |= _ISpunct | _ISgraph;
            }
            if ((b & _ISprint) && !(b & _ISspace)) {
                b |= _ISgraph;
            }
        }
        g_aCtypeB[i] = b;
    }
    g_fInit = 1;
}

const unsigned short **
__ctype_b_loc(void)
{
    static const unsigned short *p;

    ctype_loc_init();
    p = &g_aCtypeB[128];
    return &p;
}

const int **
__ctype_tolower_loc(void)
{
    static const int *p;

    ctype_loc_init();
    p = &g_aTolower[128];
    return &p;
}

const int **
__ctype_toupper_loc(void)
{
    static const int *p;

    ctype_loc_init();
    p = &g_aToupper[128];
    return &p;
}

/* glibc also exports __ctype_get_mb_cur_max for locale. */
size_t
__ctype_get_mb_cur_max(void)
{
    return 1; /* C locale */
}
