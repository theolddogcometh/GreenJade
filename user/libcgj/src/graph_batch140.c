/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch140: Z85 encode / decode (ZeroMQ RFC 32).
 * Clean-room freestanding pure C (integer/pointer; builds with -msse2).
 * No third-party ZeroMQ / CZMQ source was copied.
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   base85_encode / base85_decode / ascii85_*  → graph_batch68.c (Adobe)
 *   base64_encode / base64_decode              → graph_batch40.c
 *   base32_encode / hex_encode                 → graph_batch41.c
 *   crockford32_encode / crockford32_decode    → graph_batch119.c
 *
 * This TU adds only unique symbols:
 *   ssize_t z85_encode(const unsigned char *in, size_t inlen,
 *                      char *out, size_t outcap);
 *   ssize_t z85_decode(const char *in, size_t inlen,
 *                      unsigned char *out, size_t outcap);
 *   __z85_encode / __z85_decode  (aliases)
 *   __libcgj_batch140_marker = "libcgj-batch140"
 *
 * Z85 alphabet (RFC 32, 85 printable ASCII symbols):
 *   0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ
 *   .-:+=^! / * ?&<>()[]{}@%$#   (slash and star are alphabet members)
 *
 * Frame bounds (RFC 32):
 *   encode: inlen % 4 == 0  (4 binary bytes → 5 symbols)
 *   decode: inlen % 5 == 0  (5 symbols → 4 binary bytes)
 *
 * Encode API (matches base85 / crockford32 graph shape):
 *   out == NULL  → returns bytes needed including trailing NUL
 *   success      → returns encoded length excluding NUL
 *   error        → -1 (EINVAL / ENOSPC / EOVERFLOW)
 *
 * Decode: strict alphabet (no whitespace skip); returns decoded byte
 * count, or -1 with errno. out == NULL counts without writing.
 *
 * RFC 32 test vector:
 *   0x86 0x4F 0xD2 0x6F 0xB5 0x59 0xF7 0x5B  →  "HelloWorld"
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

const char __libcgj_batch140_marker[] = "libcgj-batch140";

/* ZeroMQ RFC 32 encoder alphabet (exactly 85 symbols). */
static const char s_b140_enc[85] =
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    ".-:+=^!/*?&<>()[]{}@%$#";

/*
 * Decoder map for full 8-bit range: 0xFF = invalid, else digit 0..84.
 * Built once from s_b140_enc so the table stays reviewable.
 */
static unsigned char s_b140_dec[256];
static int s_b140_dec_ready;

static void
b140_init_dec(void)
{
	unsigned i;

	if (s_b140_dec_ready) {
		return;
	}
	for (i = 0u; i < 256u; i++) {
		s_b140_dec[i] = 0xFFu;
	}
	for (i = 0u; i < 85u; i++) {
		s_b140_dec[(unsigned char)s_b140_enc[i]] = (unsigned char)i;
	}
	s_b140_dec_ready = 1;
}

/* Encoded symbol count for cbIn (must be multiple of 4): (cbIn/4)*5. */
static size_t
b140_enc_len(size_t cbIn)
{
	return (cbIn / 4u) * 5u;
}

/*
 * Encode one 4-byte big-endian group to 5 Z85 symbols.
 * value is a 32-bit quantity; emit high base-85 digits first.
 */
static void
b140_enc_group(uint32_t u, char *pOut)
{
	unsigned d0, d1, d2, d3, d4;

	d4 = (unsigned)(u % 85u);
	u /= 85u;
	d3 = (unsigned)(u % 85u);
	u /= 85u;
	d2 = (unsigned)(u % 85u);
	u /= 85u;
	d1 = (unsigned)(u % 85u);
	u /= 85u;
	d0 = (unsigned)u; /* 0..84 by construction for u < 2^32 */

	pOut[0] = s_b140_enc[d0];
	pOut[1] = s_b140_enc[d1];
	pOut[2] = s_b140_enc[d2];
	pOut[3] = s_b140_enc[d3];
	pOut[4] = s_b140_enc[d4];
}

/*
 * Decode 5 Z85 symbols to a 32-bit big-endian value.
 * Returns 0 on success, -1 on invalid digit or base-85 overflow.
 */
static int
b140_dec_group(const unsigned char *pDig, uint32_t *pVal)
{
	uint64_t u = 0u;
	unsigned i;

	for (i = 0u; i < 5u; i++) {
		unsigned char d = s_b140_dec[pDig[i]];

		if (d == 0xFFu) {
			return -1;
		}
		u = u * 85ull + (uint64_t)d;
		if (u > 0xffffffffull) {
			return -1;
		}
	}
	*pVal = (uint32_t)u;
	return 0;
}

/*
 * z85_encode — ZeroMQ RFC 32 binary → Z85 text.
 * inlen must be a multiple of 4 (including zero).
 */
ssize_t
z85_encode(const unsigned char *pIn, size_t cbIn, char *szOut, size_t cbOut)
{
	size_t cbSym;
	size_t cbNeed;
	size_t i;
	size_t j;

	if (pIn == NULL && cbIn != 0u) {
		errno = EINVAL;
		return -1;
	}
	if ((cbIn % 4u) != 0u) {
		errno = EINVAL;
		return -1;
	}

	/* Overflow guard: (cbIn/4)*5 + 1 for NUL. */
	if (cbIn > (SIZE_MAX / 5u) * 4u) {
		errno = EOVERFLOW;
		return -1;
	}
	cbSym = b140_enc_len(cbIn);
	if (cbSym + 1u < cbSym) {
		errno = EOVERFLOW;
		return -1;
	}
	cbNeed = cbSym + 1u;

	if (szOut == NULL) {
		return (ssize_t)cbNeed;
	}
	if (cbOut < cbNeed) {
		errno = ENOSPC;
		return -1;
	}

	j = 0u;
	for (i = 0u; i + 4u <= cbIn; i += 4u) {
		uint32_t u = ((uint32_t)pIn[i] << 24) |
		             ((uint32_t)pIn[i + 1u] << 16) |
		             ((uint32_t)pIn[i + 2u] << 8) |
		             (uint32_t)pIn[i + 3u];

		b140_enc_group(u, szOut + j);
		j += 5u;
	}

	szOut[j] = '\0';
	return (ssize_t)j;
}

/*
 * z85_decode — ZeroMQ RFC 32 Z85 text → binary.
 * inlen must be a multiple of 5 (including zero). Strict alphabet.
 */
ssize_t
z85_decode(const char *szIn, size_t cbIn, unsigned char *pOut, size_t cbOut)
{
	size_t i;
	size_t j;

	if (szIn == NULL && cbIn != 0u) {
		errno = EINVAL;
		return -1;
	}
	if ((cbIn % 5u) != 0u) {
		errno = EINVAL;
		return -1;
	}

	b140_init_dec();

	j = 0u;
	for (i = 0u; i + 5u <= cbIn; i += 5u) {
		uint32_t u;
		const unsigned char *pDig =
		    (const unsigned char *)(const void *)(szIn + i);

		if (b140_dec_group(pDig, &u) != 0) {
			errno = EINVAL;
			return -1;
		}
		if (pOut == NULL) {
			j += 4u;
			continue;
		}
		if (j + 4u > cbOut) {
			errno = ENOSPC;
			return -1;
		}
		pOut[j++] = (unsigned char)(u >> 24);
		pOut[j++] = (unsigned char)(u >> 16);
		pOut[j++] = (unsigned char)(u >> 8);
		pOut[j++] = (unsigned char)u;
	}

	return (ssize_t)j;
}

ssize_t __z85_encode(const unsigned char *p, size_t n, char *s, size_t m)
    __attribute__((alias("z85_encode")));
ssize_t __z85_decode(const char *s, size_t n, unsigned char *p, size_t m)
    __attribute__((alias("z85_decode")));
