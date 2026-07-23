/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room gmon/profil stubs. Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <sys/gmon.h>. Constants and type sizes
 * follow Linux LP64 (x86_64 / aarch64 product) where ABI numbers matter.
 *
 * Design notes
 * ------------
 * Pure C11 headers only — no inline runtime beyond macros. Implementations
 * live under user/libcgj/src/. Symbol versions are described by libc.map
 * (GLIBC_2.* nodes) for staged libc.so.6.
 *
 * Non-goals
 * ---------
 * Full POSIX/Linux completeness; stubs and soft fills may return ENOSYS
 * until the hybrid ABI path is wired. See docs/GLIBC_COMPAT.md.
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
