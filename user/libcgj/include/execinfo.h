/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room backtrace stubs (bring-up). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <execinfo.h>. Constants and type sizes
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
 * Soft residual (C2 libcgj execinfo.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = backtrace/backtrace_symbols/backtrace_symbols_fd (+ __ aliases);
 *              bring-up stubs may return empty/ENOSYS until unwind is wired
 *   product  = UDX/DDI + hot/cold ABI hosts; not freestanding class; not .ko AC
 *   honesty  = soft backtrace != product debugger; agent PASS != Dual DoD close;
 *              stamp-free bar honesty (no version stamp)
 *   law      = dual MIT OR Apache-2.0; no GPL; H2 no stamp storms
 * greppable: libcgj: execinfo soft residual Soft!=product
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int   backtrace(void **ppBuffer, int nSize);
char **backtrace_symbols(void *const *ppBuffer, int nSize);
void  backtrace_symbols_fd(void *const *ppBuffer, int nSize, int nFd);
int   __backtrace(void **ppBuffer, int nSize);
char **__backtrace_symbols(void *const *ppBuffer, int nSize);
void  __backtrace_symbols_fd(void *const *ppBuffer, int nSize, int nFd);

#ifdef __cplusplus
}
#endif
