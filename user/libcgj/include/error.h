/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room GNU error.h (subset). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <error.h>. Constants and type sizes
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
 *
 * Soft residual (C2 libcgj error.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = error/error_at_line + error_message_count/one_per_line hooks
 *   product  = UDX/DDI + hot/cold ABI hosts; not freestanding class; not .ko AC
 *   honesty  = GNU error printers != product assurance; agent PASS != Dual DoD
 *              close; stamp-free bar honesty (no version stamp)
 *   law      = dual MIT OR Apache-2.0; no GPL; H2 no stamp storms
 * greppable: libcgj: error soft residual Soft!=product
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned int error_message_count;
extern int error_one_per_line;
extern void (*error_print_progname)(void);

void error(int nStatus, int nErrnum, const char *szFmt, ...)
    __attribute__((format(printf, 3, 4)));
void error_at_line(int nStatus, int nErrnum, const char *szFile,
                   unsigned nLine, const char *szFmt, ...)
    __attribute__((format(printf, 5, 6)));

#ifdef __cplusplus
}
#endif
