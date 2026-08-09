/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room keyutils-shaped keyctl.h (subset). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <keyutils.h>. Constants and type sizes
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
 * Soft residual (C2 libcgj keyutils.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = add_key/request_key/keyctl-shaped wrappers (subset; ENOSYS soft OK)
 *   product  = Cap/key path via kernel doors not this header alone; not .ko AC
 *   honesty  = keyctl soft != product Cap mint close; agent PASS != Dual DoD
 *              close; stamp-free bar honesty (no version stamp)
 *   law      = dual MIT OR Apache-2.0; no GPL; H2 no stamp storms
 * greppable: libcgj: keyutils soft residual Soft!=product
 */
#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
long add_key(const char *szType, const char *szDesc, const void *pPayload,
             size_t cb, int nKeyring);
long request_key(const char *szType, const char *szDesc, const char *szCallout,
                 int nDestKeyring);
long keyctl(int nCmd, unsigned long a1, unsigned long a2, unsigned long a3,
            unsigned long a4);
#ifdef __cplusplus
}
#endif
