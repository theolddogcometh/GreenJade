/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped grp.h (stub surface). Not GNU glibc.
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
