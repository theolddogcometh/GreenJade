/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch128: minimal CBOR (RFC 8949) encode/decode
 * for unsigned integers, byte strings, and text strings.
 * Clean-room freestanding pure C — integer/pointer only (builds under
 * userspace -msse2). No third-party CBOR source was copied.
 *
 * Surface (unique symbols):
 *   int gj_cbor_encode_uint(uint64_t v, unsigned char *out, size_t outcap,
 *                           size_t *n);
 *   int gj_cbor_encode_bytes(const void *data, size_t dlen,
 *                            unsigned char *out, size_t outcap, size_t *n);
 *   int gj_cbor_encode_text(const void *data, size_t dlen,
 *                           unsigned char *out, size_t outcap, size_t *n);
 *   int gj_cbor_decode_uint(const unsigned char *in, size_t len,
 *                           uint64_t *v, size_t *n);
 *   int gj_cbor_decode_bytes(const unsigned char *in, size_t len,
 *                            const unsigned char **data, size_t *dlen,
 *                            size_t *n);
 *   int gj_cbor_decode_text(const unsigned char *in, size_t len,
 *                           const unsigned char **data, size_t *dlen,
 *                           size_t *n);
 *   __gj_cbor_encode_uint / __gj_cbor_encode_bytes / __gj_cbor_encode_text
 *   __gj_cbor_decode_uint / __gj_cbor_decode_bytes / __gj_cbor_decode_text
 *   __libcgj_batch128_marker = "libcgj-batch128"
 *
 * Encoding (definite-length only; no indefinite/break):
 *   Header = (major << 5) | ai
 *   Major 0 = unsigned int, 2 = byte string, 3 = text string.
 *   ai 0..23  value in ai
 *   ai 24     1-byte big-endian follow
 *   ai 25     2-byte big-endian follow
 *   ai 26     4-byte big-endian follow
 *   ai 27     8-byte big-endian follow
 *
 * Return: 0 on success, -1 on error (NULL args, truncated input, wrong
 * major type, insufficient outcap). On success *n is bytes written
 * (encode) or consumed (decode). Decode of bytes/text points *data into
 * the input buffer (no copy).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch128_marker[] = "libcgj-batch128";

/* CBOR major types (RFC 8949 §3). */
#define B128_MT_UINT  0u
#define B128_MT_BYTES 2u
#define B128_MT_TEXT  3u

/* Additional-info thresholds. */
#define B128_AI_1B  24u
#define B128_AI_2B  25u
#define B128_AI_4B  26u
#define B128_AI_8B  27u

/* ---- freestanding helpers ---------------------------------------------- */

static void
b128_put_be16(unsigned char *p, uint16_t v)
{
	p[0] = (unsigned char)((v >> 8) & 0xffu);
	p[1] = (unsigned char)(v & 0xffu);
}

static void
b128_put_be32(unsigned char *p, uint32_t v)
{
	p[0] = (unsigned char)((v >> 24) & 0xffu);
	p[1] = (unsigned char)((v >> 16) & 0xffu);
	p[2] = (unsigned char)((v >> 8) & 0xffu);
	p[3] = (unsigned char)(v & 0xffu);
}

static void
b128_put_be64(unsigned char *p, uint64_t v)
{
	p[0] = (unsigned char)((v >> 56) & 0xffu);
	p[1] = (unsigned char)((v >> 48) & 0xffu);
	p[2] = (unsigned char)((v >> 40) & 0xffu);
	p[3] = (unsigned char)((v >> 32) & 0xffu);
	p[4] = (unsigned char)((v >> 24) & 0xffu);
	p[5] = (unsigned char)((v >> 16) & 0xffu);
	p[6] = (unsigned char)((v >> 8) & 0xffu);
	p[7] = (unsigned char)(v & 0xffu);
}

static uint16_t
b128_get_be16(const unsigned char *p)
{
	return (uint16_t)(((uint16_t)p[0] << 8) | (uint16_t)p[1]);
}

static uint32_t
b128_get_be32(const unsigned char *p)
{
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
	       ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static uint64_t
b128_get_be64(const unsigned char *p)
{
	return ((uint64_t)p[0] << 56) | ((uint64_t)p[1] << 48) |
	       ((uint64_t)p[2] << 40) | ((uint64_t)p[3] << 32) |
	       ((uint64_t)p[4] << 24) | ((uint64_t)p[5] << 16) |
	       ((uint64_t)p[6] << 8) | (uint64_t)p[7];
}

static void
b128_memcpy(unsigned char *dst, const unsigned char *src, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		dst[i] = src[i];
	}
}

/*
 * Size of the argument encoding for value v (header + follow bytes).
 * Always the shortest form per RFC 8949 preferred serialization.
 */
static size_t
b128_arg_size(uint64_t v)
{
	if (v < 24u) {
		return 1u;
	}
	if (v <= 0xffu) {
		return 2u;
	}
	if (v <= 0xffffu) {
		return 3u;
	}
	if (v <= 0xffffffffu) {
		return 5u;
	}
	return 9u;
}

/*
 * Write major-type header + argument for v into out[0..).
 * Caller guarantees outcap >= b128_arg_size(v). Returns bytes written.
 */
static size_t
b128_encode_head(unsigned major, uint64_t v, unsigned char *out)
{
	unsigned char mt = (unsigned char)((major & 7u) << 5);

	if (v < 24u) {
		out[0] = (unsigned char)(mt | (unsigned char)v);
		return 1u;
	}
	if (v <= 0xffu) {
		out[0] = (unsigned char)(mt | B128_AI_1B);
		out[1] = (unsigned char)v;
		return 2u;
	}
	if (v <= 0xffffu) {
		out[0] = (unsigned char)(mt | B128_AI_2B);
		b128_put_be16(out + 1, (uint16_t)v);
		return 3u;
	}
	if (v <= 0xffffffffu) {
		out[0] = (unsigned char)(mt | B128_AI_4B);
		b128_put_be32(out + 1, (uint32_t)v);
		return 5u;
	}
	out[0] = (unsigned char)(mt | B128_AI_8B);
	b128_put_be64(out + 1, v);
	return 9u;
}

/*
 * Decode major type + argument from in[0..len).
 * On success: *pMajor, *pArg, *pHead = header+arg byte count; return 0.
 * Rejects indefinite-length (ai 31) and reserved ai 28..30.
 */
static int
b128_decode_head(const unsigned char *in, size_t len, unsigned *pMajor,
		 uint64_t *pArg, size_t *pHead)
{
	unsigned char b0;
	unsigned ai;
	uint64_t arg;
	size_t head;

	if (in == NULL || len == 0u || pMajor == NULL || pArg == NULL ||
	    pHead == NULL) {
		return -1;
	}

	b0 = in[0];
	*pMajor = (unsigned)(b0 >> 5);
	ai = (unsigned)(b0 & 0x1fu);

	if (ai < 24u) {
		arg = (uint64_t)ai;
		head = 1u;
	} else if (ai == B128_AI_1B) {
		if (len < 2u) {
			return -1;
		}
		arg = (uint64_t)in[1];
		head = 2u;
	} else if (ai == B128_AI_2B) {
		if (len < 3u) {
			return -1;
		}
		arg = (uint64_t)b128_get_be16(in + 1);
		head = 3u;
	} else if (ai == B128_AI_4B) {
		if (len < 5u) {
			return -1;
		}
		arg = (uint64_t)b128_get_be32(in + 1);
		head = 5u;
	} else if (ai == B128_AI_8B) {
		if (len < 9u) {
			return -1;
		}
		arg = b128_get_be64(in + 1);
		head = 9u;
	} else {
		/* ai 28..30 reserved; ai 31 indefinite — not supported. */
		return -1;
	}

	*pArg = arg;
	*pHead = head;
	return 0;
}

/*
 * Encode a definite-length byte/text string (major 2 or 3).
 */
static int
b128_encode_blob(unsigned major, const void *data, size_t dlen,
		 unsigned char *out, size_t outcap, size_t *n)
{
	size_t head;
	size_t need;
	const unsigned char *src;

	if (n == NULL) {
		return -1;
	}
	*n = 0u;

	if (out == NULL) {
		return -1;
	}
	/* Empty blob is legal; data may be NULL only when dlen == 0. */
	if (data == NULL && dlen != 0u) {
		return -1;
	}

	head = b128_arg_size((uint64_t)dlen);
	need = head + dlen;
	if (need < head || need > outcap) {
		/* Overflow or insufficient capacity. */
		return -1;
	}

	(void)b128_encode_head(major, (uint64_t)dlen, out);
	if (dlen != 0u) {
		src = (const unsigned char *)data;
		b128_memcpy(out + head, src, dlen);
	}
	*n = need;
	return 0;
}

/*
 * Decode a definite-length byte/text string (expected major).
 * *data points into in (no copy); *dlen = payload length; *n = total
 * bytes consumed (header + payload).
 */
static int
b128_decode_blob(unsigned expect_mt, const unsigned char *in, size_t len,
		 const unsigned char **data, size_t *dlen, size_t *n)
{
	unsigned major;
	uint64_t arg;
	size_t head;
	size_t total;

	if (n != NULL) {
		*n = 0u;
	}
	if (data != NULL) {
		*data = NULL;
	}
	if (dlen != NULL) {
		*dlen = 0u;
	}

	if (in == NULL || data == NULL || dlen == NULL || n == NULL) {
		return -1;
	}
	if (b128_decode_head(in, len, &major, &arg, &head) != 0) {
		return -1;
	}
	if (major != expect_mt) {
		return -1;
	}
	/* Payload length must fit in size_t and remaining buffer. */
	if (arg > (uint64_t)(~(size_t)0)) {
		return -1;
	}
	total = head + (size_t)arg;
	if (total < head || total > len) {
		return -1;
	}

	*data = in + head;
	*dlen = (size_t)arg;
	*n = total;
	return 0;
}

/* ---- public API -------------------------------------------------------- */

/*
 * Encode unsigned integer (major type 0) with preferred serialization.
 * Writes to out; *n = bytes written. Returns 0 or -1.
 */
int
gj_cbor_encode_uint(uint64_t v, unsigned char *out, size_t outcap, size_t *n)
{
	size_t need;

	if (n == NULL) {
		return -1;
	}
	*n = 0u;
	if (out == NULL) {
		return -1;
	}

	need = b128_arg_size(v);
	if (need > outcap) {
		return -1;
	}
	*n = b128_encode_head(B128_MT_UINT, v, out);
	return 0;
}

/*
 * Encode byte string (major type 2), definite length.
 */
int
gj_cbor_encode_bytes(const void *data, size_t dlen, unsigned char *out,
		     size_t outcap, size_t *n)
{
	return b128_encode_blob(B128_MT_BYTES, data, dlen, out, outcap, n);
}

/*
 * Encode text string (major type 3), definite length. Contents are not
 * validated as UTF-8 (caller responsibility per minimal surface).
 */
int
gj_cbor_encode_text(const void *data, size_t dlen, unsigned char *out,
		    size_t outcap, size_t *n)
{
	return b128_encode_blob(B128_MT_TEXT, data, dlen, out, outcap, n);
}

/*
 * Decode unsigned integer (major type 0). *v = value; *n = bytes consumed.
 */
int
gj_cbor_decode_uint(const unsigned char *in, size_t len, uint64_t *v,
		    size_t *n)
{
	unsigned major;
	uint64_t arg;
	size_t head;

	if (n != NULL) {
		*n = 0u;
	}
	if (v == NULL || n == NULL) {
		return -1;
	}
	if (b128_decode_head(in, len, &major, &arg, &head) != 0) {
		return -1;
	}
	if (major != B128_MT_UINT) {
		return -1;
	}
	*v = arg;
	*n = head;
	return 0;
}

/*
 * Decode byte string (major type 2). *data points into in.
 */
int
gj_cbor_decode_bytes(const unsigned char *in, size_t len,
		     const unsigned char **data, size_t *dlen, size_t *n)
{
	return b128_decode_blob(B128_MT_BYTES, in, len, data, dlen, n);
}

/*
 * Decode text string (major type 3). *data points into in; UTF-8 not checked.
 */
int
gj_cbor_decode_text(const unsigned char *in, size_t len,
		    const unsigned char **data, size_t *dlen, size_t *n)
{
	return b128_decode_blob(B128_MT_TEXT, in, len, data, dlen, n);
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_cbor_encode_uint(uint64_t v, unsigned char *out, size_t outcap,
			  size_t *n)
	__attribute__((alias("gj_cbor_encode_uint")));

int __gj_cbor_encode_bytes(const void *data, size_t dlen, unsigned char *out,
			   size_t outcap, size_t *n)
	__attribute__((alias("gj_cbor_encode_bytes")));

int __gj_cbor_encode_text(const void *data, size_t dlen, unsigned char *out,
			  size_t outcap, size_t *n)
	__attribute__((alias("gj_cbor_encode_text")));

int __gj_cbor_decode_uint(const unsigned char *in, size_t len, uint64_t *v,
			  size_t *n)
	__attribute__((alias("gj_cbor_decode_uint")));

int __gj_cbor_decode_bytes(const unsigned char *in, size_t len,
			   const unsigned char **data, size_t *dlen, size_t *n)
	__attribute__((alias("gj_cbor_decode_bytes")));

int __gj_cbor_decode_text(const unsigned char *in, size_t len,
			  const unsigned char **data, size_t *dlen, size_t *n)
	__attribute__((alias("gj_cbor_decode_text")));
