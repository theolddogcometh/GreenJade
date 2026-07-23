/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room gmon/profil stubs. Not GNU glibc.
 */
#pragma once

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct gmonparam {
    long  state;
    unsigned short *kcount;
    unsigned long   kcountsize;
    unsigned short *froms;
    unsigned long   fromssize;
    struct tostruct *tos;
    unsigned long   tossize;
    long            tolimit;
    unsigned long  *pc;
    unsigned long   textsize;
    unsigned long   hashfraction;
    long            log_hashfraction;
};

int  monstartup(unsigned long uLow, unsigned long uHigh);
void _mcleanup(void);
int  profil(unsigned short *pBuf, size_t cbBuf, size_t uOffset, unsigned uScale);

#ifdef __cplusplus
}
#endif
