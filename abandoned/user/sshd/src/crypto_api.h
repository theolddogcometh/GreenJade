/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal SUPERCOP / OpenSSH crypto_api.h for vendored ed25519.c.
 * Types and decls: public domain (Markus Friedl / SUPERCOP assembly).
 * SHA-512 backend is product MIT OR Apache-2.0 (ssh_crypto.c).
 * No OpenSSL. No includes.h. Freestanding.
 */
#ifndef GJ_SSHD_CRYPTO_API_H
#define GJ_SSHD_CRYPTO_API_H

#include <stddef.h>
#include <stdint.h>
#include <gj/string.h>

typedef int8_t crypto_int8;
typedef uint8_t crypto_uint8;
typedef int16_t crypto_int16;
typedef uint16_t crypto_uint16;
typedef int32_t crypto_int32;
typedef uint32_t crypto_uint32;
typedef int64_t crypto_int64;
typedef uint64_t crypto_uint64;

#define memcpy gj_memcpy
#define memmove gj_memmove
#define memset gj_memset

static inline void
explicit_bzero(void *p, size_t cb)
{
	(void)gj_memset(p, 0, cb);
}

static inline void
randombytes(unsigned char *pBuf, unsigned long long cb)
{
	/* Unused: product hostkey uses keypair_from_seed only. */
	(void)gj_memset(pBuf, 0, (size_t)cb);
}

#define crypto_hash_sha512_BYTES 64U

int crypto_hash_sha512(unsigned char *pOut, const unsigned char *pIn,
		       unsigned long long cbIn);

#define crypto_sign_ed25519_SECRETKEYBYTES 64U
#define crypto_sign_ed25519_PUBLICKEYBYTES 32U
#define crypto_sign_ed25519_SEEDBYTES 32U
#define crypto_sign_ed25519_BYTES 64U

int crypto_sign_ed25519(unsigned char *, unsigned long long *,
			const unsigned char *, unsigned long long,
			const unsigned char *);
int crypto_sign_ed25519_open(unsigned char *, unsigned long long *,
			     const unsigned char *, unsigned long long,
			     const unsigned char *);
int crypto_sign_ed25519_keypair(unsigned char *, unsigned char *);
int crypto_sign_ed25519_keypair_from_seed(unsigned char *, unsigned char *,
					  const unsigned char *);

#endif
