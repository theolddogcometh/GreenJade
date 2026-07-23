/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <crypt.h> stub surface for libcgj (GreenJade freestanding libc).
 * Not GNU libcrypt / glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * crypt/crypt_r and related soft-hash helpers used by legacy graphs. Product
 * policy is bring-up compatibility, not a modern password-hashing suite.
 *
 * Design notes
 * ------------
 * Traditional DES-shaped 13-char outputs may be MD5-folded soft fills — not
 * for production authentication. Prefer platform keystores for real secrets.
 *
 * Non-goals
 * ---------
 * bcrypt/scrypt/argon2; shipping GPL crypt implementations.
 * See docs/GLIBC_COMPAT.md.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

char *crypt(const char *szKey, const char *szSalt);
char *crypt_r(const char *szKey, const char *szSalt, void *pData);

#ifdef __cplusplus
}
#endif
