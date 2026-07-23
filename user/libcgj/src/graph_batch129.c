/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch129: MessagePack fixstr / positive fixint / bin8.
 * Clean-room freestanding pure C — builds under userspace -msse2.
 * No third-party MessagePack source was copied.
 *
 * Wire subset (https://msgpack.org/ format family):
 *   positive fixint  0x00..0x7f     single-byte unsigned 0..127
 *   fixstr           0xa0|len       len in 0..31, then len payload bytes
 *   bin8             0xc4 u8len     u8len in 0..255, then u8len payload bytes
 *
 * Surface (unique symbols):
 *   int gj_msgpack_encode_str(const char *s, unsigned char *out,
 *                             size_t outcap, size_t *n);
 *     Encode NUL-terminated s. len<=31 → fixstr; 32..255 → bin8;
 *     longer → -1. *n = bytes written (or required when out is NULL /
 *     outcap too small). Empty string (s=="" or s==NULL) → fixstr len 0.
 *   int gj_msgpack_encode_uint(uint64_t v, unsigned char *out,
 *                              size_t outcap, size_t *n);
 *     Encode as positive fixint when v<=127; else -1 (subset limit).
 *   int gj_msgpack_decode_str(const unsigned char *in, size_t len,
 *                             const char **s, size_t *slen, size_t *n);
 *     Decode fixstr or bin8. *s points into in (zero-copy, not necessarily
 *     NUL-terminated); *slen = payload length; *n = total bytes consumed.
 *   __gj_msgpack_encode_str / __gj_msgpack_encode_uint /
 *   __gj_msgpack_decode_str  (aliases)
 *   __libcgj_batch129_marker = "libcgj-batch129"
 *
 * Semantics: 0 success, -1 error. *n is written on every path where the
 * size is known (required size on capacity failure / size-query).
 */

#include <stddef.h>
#include <stdint.h>

#if defined(__x86_64__) || defined(__i386__)
#include <emmintrin.h>
#endif

const char __libcgj_batch129_marker[] = "libcgj-batch129";

/* MessagePack markers used by this subset. */
#define B129_FIXSTR_MASK   0xe0u
#define B129_FIXSTR_TAG    0xa0u
#define B129_FIXINT_MAX    0x7fu
#define B129_BIN8          0xc4u
#define B129_FIXSTR_MAXLEN 31u
#define B129_BIN8_MAXLEN   255u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b129_strlen(const char *sz)
{
	size_t cb = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[cb] != '\0') {
		cb++;
	}
	return cb;
}

static void
b129_copy(unsigned char *pDst, const unsigned char *pSrc, size_t cb)
{
	size_t i = 0u;

	if (pDst == NULL || pSrc == NULL || cb == 0u) {
		return;
	}

#if defined(__x86_64__) || defined(__i386__)
	if (cb >= 16u) {
		size_t nBlk = cb & ~(size_t)15u;

		while (i < nBlk) {
			__m128i v = _mm_loadu_si128(
			    (const __m128i *)(const void *)(pSrc + i));
			_mm_storeu_si128((__m128i *)(void *)(pDst + i), v);
			i += 16u;
		}
	}
#endif
	while (i < cb) {
		pDst[i] = pSrc[i];
		i++;
	}
}

/*
 * Write cbNeed bytes of MessagePack: header in aHdr[0..cbHdr), then
 * optional payload pPay[0..cbPay). Size-query when out==NULL.
 * Returns 0 / -1; always stores *pcbOut = cbNeed when pcbOut != NULL.
 */
static int
b129_emit(unsigned char *pOut, size_t cbCap, size_t *pcbOut,
          const unsigned char *aHdr, size_t cbHdr,
          const unsigned char *pPay, size_t cbPay)
{
	size_t cbNeed;

	cbNeed = cbHdr + cbPay;
	if (pcbOut != NULL) {
		*pcbOut = cbNeed;
	}
	if (pOut == NULL) {
		/* Size query: success with *pcbOut set. */
		return 0;
	}
	if (cbCap < cbNeed) {
		return -1;
	}
	b129_copy(pOut, aHdr, cbHdr);
	if (cbPay > 0u && pPay != NULL) {
		b129_copy(pOut + cbHdr, pPay, cbPay);
	}
	return 0;
}

/* ---- public: encode_str ------------------------------------------------ */

/*
 * Encode a C string as MessagePack fixstr (len<=31) or bin8 (len<=255).
 * s == NULL is treated as empty. *n must be non-NULL.
 */
int
gj_msgpack_encode_str(const char *s, unsigned char *out, size_t outcap,
                      size_t *n)
{
	const unsigned char *pPay;
	size_t cbPay;
	unsigned char aHdr[2];
	size_t cbHdr;

	if (n == NULL) {
		return -1;
	}

	cbPay = b129_strlen(s);
	pPay = (const unsigned char *)(s != NULL ? s : "");

	if (cbPay <= B129_FIXSTR_MAXLEN) {
		/* fixstr: 0xa0 | len, then payload */
		aHdr[0] = (unsigned char)(B129_FIXSTR_TAG | (unsigned)cbPay);
		cbHdr = 1u;
	} else if (cbPay <= B129_BIN8_MAXLEN) {
		/* bin8: 0xc4, u8 length, then payload */
		aHdr[0] = (unsigned char)B129_BIN8;
		aHdr[1] = (unsigned char)cbPay;
		cbHdr = 2u;
	} else {
		/* Subset cannot encode strings longer than 255. */
		*n = 0u;
		return -1;
	}

	return b129_emit(out, outcap, n, aHdr, cbHdr, pPay, cbPay);
}

/* ---- public: encode_uint ----------------------------------------------- */

/*
 * Encode v as positive fixint (single byte 0x00..0x7f). Values above 127
 * are outside this TU's wire subset.
 */
int
gj_msgpack_encode_uint(uint64_t v, unsigned char *out, size_t outcap,
                       size_t *n)
{
	unsigned char aHdr[1];

	if (n == NULL) {
		return -1;
	}
	if (v > (uint64_t)B129_FIXINT_MAX) {
		*n = 0u;
		return -1;
	}

	aHdr[0] = (unsigned char)v;
	return b129_emit(out, outcap, n, aHdr, 1u, NULL, 0u);
}

/* ---- public: decode_str ------------------------------------------------ */

/*
 * Decode fixstr or bin8 from in[0..len). Zero-copy: *s points into in.
 * *slen = payload length; *n = header+payload bytes consumed.
 * Payload is raw MessagePack bytes (not forced NUL-terminated).
 */
int
gj_msgpack_decode_str(const unsigned char *in, size_t len, const char **s,
                      size_t *slen, size_t *n)
{
	unsigned char uMk;
	size_t cbPay;
	size_t cbHdr;
	size_t cbTotal;

	if (in == NULL || s == NULL || slen == NULL || n == NULL || len == 0u) {
		return -1;
	}

	uMk = in[0];

	if ((uMk & (unsigned char)B129_FIXSTR_MASK) ==
	    (unsigned char)B129_FIXSTR_TAG) {
		/* fixstr: low 5 bits = length */
		cbPay = (size_t)(uMk & 0x1fu);
		cbHdr = 1u;
	} else if (uMk == (unsigned char)B129_BIN8) {
		if (len < 2u) {
			return -1;
		}
		cbPay = (size_t)in[1];
		cbHdr = 2u;
	} else {
		/* Not a string/bin type in this subset. */
		return -1;
	}

	cbTotal = cbHdr + cbPay;
	if (len < cbTotal) {
		return -1;
	}

	*s = (const char *)(const void *)(in + cbHdr);
	*slen = cbPay;
	*n = cbTotal;
	return 0;
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_msgpack_encode_str(const char *s, unsigned char *out, size_t outcap,
                            size_t *n)
    __attribute__((alias("gj_msgpack_encode_str")));

int __gj_msgpack_encode_uint(uint64_t v, unsigned char *out, size_t outcap,
                             size_t *n)
    __attribute__((alias("gj_msgpack_encode_uint")));

int __gj_msgpack_decode_str(const unsigned char *in, size_t len,
                            const char **s, size_t *slen, size_t *n)
    __attribute__((alias("gj_msgpack_decode_str")));
