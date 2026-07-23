/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch121: Argon2id PHC encode + verify wrappers.
 * Clean-room freestanding pure C. Builds under userspace -msse2.
 *
 * Does NOT redefine batch65 argon2id_hash / argon2i_hash (or batch46
 * *_hash_raw ENOSYS stubs). Uses extern argon2id_hash from batch65.
 *
 * This TU adds only unique symbols:
 *   int argon2id_hash_encoded(const char *pwd, size_t pwdlen,
 *                             const void *salt, size_t saltlen,
 *                             uint32_t t_cost, uint32_t m_cost_kib,
 *                             uint32_t parallelism, size_t hashlen,
 *                             char *encoded, size_t encodedlen);
 *     — PHC string: $argon2id$v=19$m=..,t=..,p=..$salt_b64$hash_b64
 *       (RFC 4648 base64 alphabet, no '=' padding — Argon2 PHC shape).
 *   int gj_argon2id_verify(const char *pwd, size_t plen,
 *                          const unsigned char *hash, size_t hlen,
 *                          const unsigned char *salt, size_t slen,
 *                          uint32_t t, uint32_t m_kib, uint32_t p);
 *     — re-hash via argon2id_hash; constant-time compare.
 *       Returns 0 match, 1 mismatch, -1 error (errno set).
 *   __argon2id_hash_encoded / __gj_argon2id_verify  (aliases)
 *   __libcgj_batch121_marker = "libcgj-batch121"
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch121_marker[] = "libcgj-batch121";

/* batch65 — do not redefine */
extern int argon2id_hash(const char *pwd, size_t pwdlen, const void *salt,
                         size_t saltlen, uint32_t t_cost, uint32_t m_cost_kib,
                         uint32_t parallelism, void *out, size_t outlen);

/* Soft cap aligned with batch65 B65_MAX_OUT (stack scratch for verify). */
#define B121_MAX_HASH 1024u
#define B121_MAX_B64  (((B121_MAX_HASH + 2u) / 3u) * 4u + 4u)

/* ---- freestanding helpers ---------------------------------------------- */

static void
b121_memzero(void *pDst, size_t cb)
{
	volatile unsigned char *p = (volatile unsigned char *)pDst;
	size_t i;

	for (i = 0u; i < cb; i++) {
		p[i] = 0;
	}
}

/* Constant-time memcmp: 0 equal, nonzero differ. */
static int
b121_ct_memcmp(const unsigned char *a, const unsigned char *b, size_t cb)
{
	unsigned char uDiff = 0;
	size_t i;

	for (i = 0u; i < cb; i++) {
		uDiff |= (unsigned char)(a[i] ^ b[i]);
	}
	return (int)uDiff;
}

/* Append raw bytes; returns 0 or -1 (ERANGE). */
static int
b121_append(char *szDst, size_t cbDst, size_t *pOff, const char *pSrc,
            size_t cbSrc)
{
	size_t i;
	size_t off;

	if (szDst == NULL || pOff == NULL || pSrc == NULL) {
		errno = EINVAL;
		return -1;
	}
	off = *pOff;
	if (cbSrc > cbDst || off > cbDst - cbSrc) {
		errno = ERANGE;
		return -1;
	}
	for (i = 0u; i < cbSrc; i++) {
		szDst[off + i] = pSrc[i];
	}
	*pOff = off + cbSrc;
	return 0;
}

/* Decimal uint32 into sz (no NUL). Returns digits written or 0 on error. */
static size_t
b121_u32_dec(char *sz, size_t cb, uint32_t u)
{
	char aRev[10];
	size_t n = 0u;
	size_t i;

	do {
		aRev[n++] = (char)('0' + (unsigned)(u % 10u));
		u /= 10u;
	} while (u != 0u && n < sizeof(aRev));

	if (n > cb) {
		return 0u;
	}
	for (i = 0u; i < n; i++) {
		sz[i] = aRev[n - 1u - i];
	}
	return n;
}

/*
 * Argon2 PHC base64: standard alphabet, no padding.
 * Writes into szOut without NUL. Returns bytes written, or (size_t)-1.
 */
static size_t
b121_b64_encode(const unsigned char *pIn, size_t cbIn, char *szOut,
                size_t cbOut)
{
	static const char s_b64[] =
	    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	size_t i = 0u;
	size_t o = 0u;

	if (pIn == NULL && cbIn != 0u) {
		return (size_t)-1;
	}
	if (szOut == NULL) {
		return (size_t)-1;
	}

	while (i + 3u <= cbIn) {
		unsigned u = ((unsigned)pIn[i] << 16) |
		             ((unsigned)pIn[i + 1u] << 8) |
		             (unsigned)pIn[i + 2u];

		if (o + 4u > cbOut) {
			return (size_t)-1;
		}
		szOut[o++] = s_b64[(u >> 18) & 63u];
		szOut[o++] = s_b64[(u >> 12) & 63u];
		szOut[o++] = s_b64[(u >> 6) & 63u];
		szOut[o++] = s_b64[u & 63u];
		i += 3u;
	}

	if (i < cbIn) {
		unsigned u = (unsigned)pIn[i] << 16;

		if (i + 1u < cbIn) {
			u |= (unsigned)pIn[i + 1u] << 8;
		}
		if (o + 2u + (i + 1u < cbIn ? 1u : 0u) > cbOut) {
			return (size_t)-1;
		}
		szOut[o++] = s_b64[(u >> 18) & 63u];
		szOut[o++] = s_b64[(u >> 12) & 63u];
		if (i + 1u < cbIn) {
			szOut[o++] = s_b64[(u >> 6) & 63u];
		}
	}

	return o;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * argon2id_hash_encoded — compute Argon2id and format PHC string.
 * encodedlen must hold the full string + NUL.
 * Returns 0 on success, -1 with errno on error.
 */
int
argon2id_hash_encoded(const char *pwd, size_t pwdlen, const void *salt,
                      size_t saltlen, uint32_t t_cost, uint32_t m_cost_kib,
                      uint32_t parallelism, size_t hashlen, char *encoded,
                      size_t encodedlen)
{
	unsigned char aHash[B121_MAX_HASH];
	char aB64[B121_MAX_B64];
	char aDec[12];
	size_t off = 0u;
	size_t nB64;
	size_t nDec;
	int nRc;

	if (encoded == NULL || encodedlen == 0u || hashlen == 0u ||
	    hashlen > B121_MAX_HASH) {
		errno = EINVAL;
		return -1;
	}
	if (salt == NULL && saltlen != 0u) {
		errno = EINVAL;
		return -1;
	}
	if (pwd == NULL && pwdlen != 0u) {
		errno = EINVAL;
		return -1;
	}

	nRc = argon2id_hash(pwd, pwdlen, salt, saltlen, t_cost, m_cost_kib,
	                    parallelism, aHash, hashlen);
	if (nRc != 0) {
		return -1;
	}

	/* $argon2id$v=19$m= */
	if (b121_append(encoded, encodedlen, &off, "$argon2id$v=19$m=", 17u) !=
	    0) {
		b121_memzero(aHash, hashlen);
		return -1;
	}
	nDec = b121_u32_dec(aDec, sizeof(aDec), m_cost_kib);
	if (nDec == 0u ||
	    b121_append(encoded, encodedlen, &off, aDec, nDec) != 0) {
		errno = ERANGE;
		b121_memzero(aHash, hashlen);
		return -1;
	}
	if (b121_append(encoded, encodedlen, &off, ",t=", 3u) != 0) {
		b121_memzero(aHash, hashlen);
		return -1;
	}
	nDec = b121_u32_dec(aDec, sizeof(aDec), t_cost);
	if (nDec == 0u ||
	    b121_append(encoded, encodedlen, &off, aDec, nDec) != 0) {
		errno = ERANGE;
		b121_memzero(aHash, hashlen);
		return -1;
	}
	if (b121_append(encoded, encodedlen, &off, ",p=", 3u) != 0) {
		b121_memzero(aHash, hashlen);
		return -1;
	}
	nDec = b121_u32_dec(aDec, sizeof(aDec), parallelism);
	if (nDec == 0u ||
	    b121_append(encoded, encodedlen, &off, aDec, nDec) != 0) {
		errno = ERANGE;
		b121_memzero(aHash, hashlen);
		return -1;
	}
	if (b121_append(encoded, encodedlen, &off, "$", 1u) != 0) {
		b121_memzero(aHash, hashlen);
		return -1;
	}

	/* salt b64 */
	nB64 = b121_b64_encode((const unsigned char *)salt, saltlen, aB64,
	                       sizeof(aB64));
	if (nB64 == (size_t)-1 ||
	    b121_append(encoded, encodedlen, &off, aB64, nB64) != 0) {
		errno = ERANGE;
		b121_memzero(aHash, hashlen);
		return -1;
	}
	if (b121_append(encoded, encodedlen, &off, "$", 1u) != 0) {
		b121_memzero(aHash, hashlen);
		return -1;
	}

	/* hash b64 */
	nB64 = b121_b64_encode(aHash, hashlen, aB64, sizeof(aB64));
	if (nB64 == (size_t)-1 ||
	    b121_append(encoded, encodedlen, &off, aB64, nB64) != 0) {
		errno = ERANGE;
		b121_memzero(aHash, hashlen);
		return -1;
	}

	/* NUL terminate */
	if (off >= encodedlen) {
		errno = ERANGE;
		b121_memzero(aHash, hashlen);
		return -1;
	}
	encoded[off] = '\0';
	b121_memzero(aHash, hashlen);
	return 0;
}

/*
 * gj_argon2id_verify — recompute Argon2id and constant-time compare.
 * Returns 0 on match, 1 on mismatch, -1 on error (errno set).
 */
int
gj_argon2id_verify(const char *pwd, size_t plen, const unsigned char *hash,
                   size_t hlen, const unsigned char *salt, size_t slen,
                   uint32_t t, uint32_t m_kib, uint32_t p)
{
	unsigned char aGot[B121_MAX_HASH];
	int nRc;
	int nDiff;

	if (hash == NULL || hlen == 0u || hlen > B121_MAX_HASH) {
		errno = EINVAL;
		return -1;
	}
	if (salt == NULL && slen != 0u) {
		errno = EINVAL;
		return -1;
	}
	if (pwd == NULL && plen != 0u) {
		errno = EINVAL;
		return -1;
	}

	nRc = argon2id_hash(pwd, plen, salt, slen, t, m_kib, p, aGot, hlen);
	if (nRc != 0) {
		return -1;
	}

	nDiff = b121_ct_memcmp(aGot, hash, hlen);
	b121_memzero(aGot, hlen);
	return nDiff != 0 ? 1 : 0;
}

int __argon2id_hash_encoded(const char *pwd, size_t pwdlen, const void *salt,
                            size_t saltlen, uint32_t t_cost,
                            uint32_t m_cost_kib, uint32_t parallelism,
                            size_t hashlen, char *encoded, size_t encodedlen)
    __attribute__((alias("argon2id_hash_encoded")));

int __gj_argon2id_verify(const char *pwd, size_t plen,
                         const unsigned char *hash, size_t hlen,
                         const unsigned char *salt, size_t slen, uint32_t t,
                         uint32_t m_kib, uint32_t p)
    __attribute__((alias("gj_argon2id_verify")));
