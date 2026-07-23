/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped ftw/nftw (light). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <ftw.h>. Constants and type sizes
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

#include <sys/stat.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FTW_F   0 /* file */
#define FTW_D   1 /* directory */
#define FTW_DNR 2 /* directory not readable */
#define FTW_NS  3 /* stat failed */
#define FTW_SL  4 /* symlink */
#define FTW_DP  5 /* directory post-order */
#define FTW_SLN 6 /* dangling symlink */

#define FTW_PHYS  1
#define FTW_MOUNT 2
#define FTW_DEPTH 8
#define FTW_CHDIR 4

struct FTW {
    int base;
    int level;
};

int ftw(const char *szPath,
        int (*pfn)(const char *, const struct stat *, int), int nFdLimit);
int nftw(const char *szPath,
         int (*pfn)(const char *, const struct stat *, int, struct FTW *),
         int nFdLimit, int nFlags);

#ifdef __cplusplus
}
#endif
