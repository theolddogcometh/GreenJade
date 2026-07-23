/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <dlfcn.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * dlopen/dlsym/dlvsym/dlclose/dlerror/dladdr/dlinfo/dlmopen and RTLD_* flags.
 * Real dynamic loading is owned by ld-gj; these symbols satisfy link + ABI
 * and may forward or soft-fill depending on process image.
 *
 * Design notes
 * ------------
 * RTLD_DEFAULT / RTLD_NEXT are sentinel handles. dl_iterate_phdr is also
 * declared via <link.h>. See user/ld-gj and docs/GLIBC_COMPAT.md.
 *
 * Non-goals
 * ---------
 * Full glibc namespace / auditor / dlmopen isolation semantics on day one.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ---- dlopen mode flags ------------------------------------------------- */

#define RTLD_LAZY   0x00001
#define RTLD_NOW    0x00002
#define RTLD_LOCAL  0x00000
#define RTLD_GLOBAL 0x00100
#define RTLD_NOLOAD 0x00004
#define RTLD_NODELETE 0x01000

#define RTLD_DEFAULT  ((void *)0)
#define RTLD_NEXT     ((void *)-1L)

/* ---- Dynamic linking API (ld-gj owns real load) ------------------------ */

void  *dlopen(const char *szFile, int nMode);
void  *dlsym(void *pHandle, const char *szSymbol);
void  *dlvsym(void *pHandle, const char *szSymbol, const char *szVersion);
int    dlclose(void *pHandle);
char  *dlerror(void);
int    dladdr(const void *pAddr, void *pInfo /* Dl_info * */);
int    dladdr1(const void *pAddr, void *pInfo, void **ppExtra, int nFlags);
int    dlinfo(void *pHandle, int nRequest, void *pArg);
void  *dlmopen(long nNs, const char *szFile, int nMode);

/* Also in <link.h>: dl_iterate_phdr */

#ifdef __cplusplus
}
#endif
