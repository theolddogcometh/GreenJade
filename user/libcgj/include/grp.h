/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <grp.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * group database accessors (getgrgid/getgrnam and friends). Bring-up may be
 * minimal (primary gid only) per docs/GLIBC_COMPAT.md.
 *
 * Design notes
 * ------------
 * Complements <pwd.h> and getgrouplist in <unistd.h>.
 *
 * Non-goals
 * ---------
 * Full NSS group enumeration backends.
 *
 * Soft residual (C2 libcgj grp.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = struct group + getgrgid/getgrnam and friends; bring-up may be
 *              primary-gid-only soft fill (not full NSS)
 *   product  = UDX/DDI + hot/cold ABI hosts; not freestanding class; not .ko AC
 *   honesty  = group DB soft != product identity completeness; agent PASS !=
 *              Dual DoD close; stamp-free bar honesty (no version stamp)
 *   law      = dual MIT OR Apache-2.0; no GPL; H2 no stamp storms
 * greppable: libcgj: grp soft residual Soft!=product
 */
#pragma once

#include <stdio.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct group {
    char   *gr_name;
    char   *gr_passwd;
    gid_t   gr_gid;
    char  **gr_mem;
};

struct group *getgrgid(gid_t gid);
struct group *getgrnam(const char *szName);
struct group *getgrent(void);
void          setgrent(void);
void          endgrent(void);
int           getgrgid_r(gid_t gid, struct group *pGrp, char *szBuf, size_t cb,
                         struct group **ppResult);
int           getgrnam_r(const char *szName, struct group *pGrp, char *szBuf,
                         size_t cb, struct group **ppResult);
int           getgrent_r(struct group *pGrp, char *szBuf, size_t cb,
                         struct group **ppResult);
struct group *fgetgrent(FILE *pF);
int           fgetgrent_r(FILE *pF, struct group *pGrp, char *szBuf, size_t cb,
                          struct group **ppResult);
int           putgrent(const struct group *pGrp, FILE *pF);

#ifdef __cplusplus
}
#endif
