/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped envz.h. Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <envz.h>. Constants and type sizes
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
 * Soft residual (C2 libcgj envz.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = envz_entry/get/add/merge/remove/strip over argz-shaped buffers
 *   product  = UDX/DDI + hot/cold ABI hosts; not freestanding class; not .ko AC
 *   honesty  = envz helpers != product env policy; agent PASS != Dual DoD close;
 *              stamp-free bar honesty (no version stamp)
 *   law      = dual MIT OR Apache-2.0; no GPL; H2 no stamp storms
 * greppable: libcgj: envz soft residual Soft!=product
 */
#pragma once

#include <argz.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

char   *envz_entry(const char *pEnvz, size_t cbEnvz, const char *szName);
char   *envz_get(const char *pEnvz, size_t cbEnvz, const char *szName);
error_t envz_add(char **ppEnvz, size_t *pcbEnvz, const char *szName,
                 const char *szValue);
error_t envz_merge(char **ppEnvz, size_t *pcbEnvz, const char *pEnvz2,
                   size_t cbEnvz2, int nOverride);
void    envz_remove(char **ppEnvz, size_t *pcbEnvz, const char *szName);
void    envz_strip(char **ppEnvz, size_t *pcbEnvz);

#ifdef __cplusplus
}
#endif
