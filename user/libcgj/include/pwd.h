/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <pwd.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * passwd database accessors (getpwuid/getpwnam and reentrant forms). Bring-up
 * may return root + self only until a real /etc/passwd path exists.
 *
 * Design notes
 * ------------
 * struct passwd field order matches Linux glibc expectations for common apps.
 *
 * Non-goals
 * ---------
 * Full NSS (LDAP/SSSD) modules inside libc.
 *
 * Soft residual (C2 libcgj pwd.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = struct passwd + getpwuid/getpwnam (+ reentrant); bring-up may be
 *              root + self only until real /etc/passwd path exists
 *   product  = UDX/DDI + hot/cold ABI hosts; not freestanding class; not .ko AC
 *   honesty  = passwd DB soft != product identity completeness; agent PASS != Dual
 *              DoD close; stamp-free bar honesty (no version stamp)
 *   law      = dual MIT OR Apache-2.0; no GPL; H2 no stamp storms
 * greppable: libcgj: pwd soft residual Soft!=product
 */
#pragma once

#include <stdio.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct passwd {
    char   *pw_name;
    char   *pw_passwd;
    uid_t   pw_uid;
    gid_t   pw_gid;
    char   *pw_gecos;
    char   *pw_dir;
    char   *pw_shell;
};

struct passwd *getpwuid(uid_t uid);
struct passwd *getpwnam(const char *szName);
struct passwd *getpwent(void);
void           setpwent(void);
void           endpwent(void);
int            getpwuid_r(uid_t uid, struct passwd *pPwd, char *szBuf,
                          size_t cb, struct passwd **ppResult);
int            getpwnam_r(const char *szName, struct passwd *pPwd, char *szBuf,
                          size_t cb, struct passwd **ppResult);
int            getpwent_r(struct passwd *pPwd, char *szBuf, size_t cb,
                          struct passwd **ppResult);
struct passwd *fgetpwent(FILE *pF);
int            fgetpwent_r(FILE *pF, struct passwd *pPwd, char *szBuf,
                           size_t cb, struct passwd **ppResult);
int            putpwent(const struct passwd *pPwd, FILE *pF);

#ifdef __cplusplus
}
#endif
