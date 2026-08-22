/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Placeholder <util.h> (OpenBSD libutil) for OpenSSH-portable packet.c /
 * sshkey.c. Host Linux has no /usr/include/util.h. Not libbsd.
 *
 * packet.c calls fmt_scaled; sshkey.c calls bcrypt_pbkdf. Declarations
 * match openbsd-compat.h (HAVE_FMT_SCALED / HAVE_BCRYPT_PBKDF unset).
 * login_getclass is login_cap(3); OpenSSH uses it only under
 * HAVE_LOGIN_CAP (unset). Stub so the include parses; do not set
 * HAVE_LOGIN_CAP.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifndef FMT_SCALED_STRSIZE
# define FMT_SCALED_STRSIZE 7
#endif

#ifndef HAVE_FMT_SCALED
int fmt_scaled(long long nNumber, char *szResult);
#endif

#ifndef HAVE_SCAN_SCALED
int scan_scaled(char *szScaled, long long *pResult);
#endif

#ifndef HAVE_BCRYPT_PBKDF
int bcrypt_pbkdf(const char *szPass, size_t cbPass,
    const uint8_t *pSalt, size_t cbSalt,
    uint8_t *pKey, size_t cbKey, unsigned int uRounds);
#endif

#ifndef HAVE_LOGIN_CAP
struct login_cap;
struct passwd;

static inline struct login_cap *
login_getclass(const char *szClass)
{
    (void)szClass;
    return NULL;
}

static inline struct login_cap *
login_getpwclass(const struct passwd *pPw)
{
    (void)pPw;
    return NULL;
}

static inline void
login_close(struct login_cap *pLc)
{
    (void)pLc;
}
#endif
