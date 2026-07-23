/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped pwd.h (stub surface). Not GNU glibc.
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
