/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch205: simple scrypt bring-up wrapper.
 *
 * Surface (unique symbols):
 *   int gj_scrypt_simple(const char *pwd, const unsigned char *salt,
 *                        size_t slen, unsigned char out[32]);
 *     — N=16 r=1 p=1 bring-up using extern scrypt if present else
 *       ENOSYS-style -1. Derives 32 bytes into out[].
 *   __gj_scrypt_simple  (alias)
 *   __libcgj_batch205_marker = "libcgj-batch205"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No third-party source copied. Does not redefine
 * scrypt / scrypt_kdf (batch46).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch205_marker[] = "libcgj-batch205";

/*
 * RFC 7914 scrypt from graph_batch46 — weak so this TU links alone when
 * batch46 is absent (freestanding smoke / single-file -c) and returns
 * ENOSYS-style -1 instead of an unresolved symbol at runtime check.
 */
extern int scrypt(const uint8_t *pPass, size_t cbPass, const uint8_t *pSalt,
                  size_t cbSalt, uint64_t uN, uint32_t uR, uint32_t uP,
                  uint8_t *pOut, size_t cbOut) __attribute__((weak));

/* Bring-up parameters: tiny memory footprint (N*128*r = 2 KiB V array). */
#define B205_N 16ull
#define B205_R 1u
#define B205_P 1u
#define B205_OUT 32u

/* ---- helpers ----------------------------------------------------------- */

static size_t
b205_strlen(const char *sz)
{
	size_t n = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/* ---- gj_scrypt_simple -------------------------------------------------- */

/*
 * Fixed-parameter scrypt: N=16, r=1, p=1 → 32-byte key into out.
 * Uses weak extern scrypt when linked (batch46); otherwise ENOSYS-style -1.
 * No errno (freestanding-safe).
 */
int
gj_scrypt_simple(const char *pwd, const unsigned char *salt, size_t slen,
                 unsigned char out[32])
{
	size_t plen;

	if (pwd == NULL || out == NULL) {
		return -1;
	}
	if (slen > 0u && salt == NULL) {
		return -1;
	}

	/* Weak ref unresolved → NULL; treat as ENOSYS. */
	if (scrypt == NULL) {
		return -1;
	}

	plen = b205_strlen(pwd);
	return scrypt((const uint8_t *)pwd, plen, (const uint8_t *)salt, slen,
	              B205_N, B205_R, B205_P, (uint8_t *)out, (size_t)B205_OUT);
}

int __gj_scrypt_simple(const char *pwd, const unsigned char *salt, size_t slen,
                       unsigned char out[32])
    __attribute__((alias("gj_scrypt_simple")));
