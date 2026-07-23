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
