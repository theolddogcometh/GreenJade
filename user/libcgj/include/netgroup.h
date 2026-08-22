/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <netgroup.h> for libcgj. OpenSSH auth-rhosts.c includes
 * this for innetgr (HAVE_INNETGR). Bring-up: empty netgroup table.
 * Dual DoD B OPEN. Soft!=product. G-AC-1. No stamp bump.
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NETGROUP
#define NETGROUP 1
#define HOST     2
#define USER     3
#define DOMAIN   4
#endif

int  innetgr(const char *szNetgroup, const char *szHost, const char *szUser,
             const char *szDomain);
void setnetgrent(const char *szNetgroup);
void endnetgrent(void);
int  getnetgrent(char **ppHost, char **ppUser, char **ppDomain);
int  getnetgrent_r(char **ppHost, char **ppUser, char **ppDomain, char *szBuf,
                   size_t cb);

#ifdef __cplusplus
}
#endif
