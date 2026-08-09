/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped nl_types.h (message catalogs). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <nl_types.h>. Constants and type sizes
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
 * Soft residual (C2 libcgj nl_types.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = nl_catd + catopen/catgets/catclose message-catalog surface
 *   product  = UDX/DDI + hot/cold ABI hosts; not freestanding class; not .ko AC
 *   honesty  = catalog soft != product i18n completeness; agent PASS != Dual DoD
 *              close; stamp-free bar honesty (no version stamp)
 *   law      = dual MIT OR Apache-2.0; no GPL; H2 no stamp storms
 * greppable: libcgj: nl_types soft residual Soft!=product
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void *nl_catd;
typedef int   nl_item;

#define NL_SETD       1
#define NL_CAT_LOCALE 1

nl_catd catopen(const char *szName, int nFlag);
char   *catgets(nl_catd cat, int nSet, int nMsg, const char *szDefault);
int     catclose(nl_catd cat);

#ifdef __cplusplus
}
#endif
