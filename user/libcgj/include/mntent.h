/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped mntent.h (bring-up). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <mntent.h>. Constants and type sizes
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
 * Soft residual (C2 libcgj mntent.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = struct mntent + setmntent/getmntent/addmntent/endmntent (bring-up)
 *   product  = store/VFS doors for real mounts; not freestanding class; not .ko AC
 *   honesty  = mntent soft != product mount completeness; agent PASS != Dual DoD
 *              close; stamp-free bar honesty (no version stamp)
 *   law      = dual MIT OR Apache-2.0; no GPL; H2 no stamp storms
 * greppable: libcgj: mntent soft residual Soft!=product
 */
#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MNTTAB   "/etc/fstab"
#define MOUNTED  "/etc/mtab"

#define MNTTYPE_IGNORE  "ignore"
#define MNTTYPE_NFS     "nfs"
#define MNTTYPE_SWAP    "swap"

#define MNTOPT_DEFAULTS "defaults"
#define MNTOPT_RO       "ro"
#define MNTOPT_RW       "rw"
#define MNTOPT_SUID     "suid"
#define MNTOPT_NOSUID   "nosuid"
#define MNTOPT_NOAUTO   "noauto"

struct mntent {
    char *mnt_fsname;
    char *mnt_dir;
    char *mnt_type;
    char *mnt_opts;
    int   mnt_freq;
    int   mnt_passno;
};

FILE          *setmntent(const char *szFile, const char *szType);
struct mntent *getmntent(FILE *pF);
struct mntent *getmntent_r(FILE *pF, struct mntent *pMnt, char *szBuf,
                           int nBufSize);
int            addmntent(FILE *pF, const struct mntent *pMnt);
int            endmntent(FILE *pF);
char          *hasmntopt(const struct mntent *pMnt, const char *szOpt);

#ifdef __cplusplus
}
#endif
