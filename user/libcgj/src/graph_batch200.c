/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch200: QUIC variable-length integer encode/decode
 * (RFC 9000 §16).
 *
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party QUIC source was copied.
 *
 * Wire format (network / big-endian; length encoded in the two MSBs of
 * the first byte):
 *   00xxxxxx                → 1 byte,  6-bit value  (0 .. 2^6  - 1)
 *   01xxxxxx xxxxxxxx       → 2 bytes, 14-bit value (0 .. 2^14 - 1)
 *   10xxxxxx ... (4 total)  → 4 bytes, 30-bit value (0 .. 2^30 - 1)
 *   11xxxxxx ... (8 total)  → 8 bytes, 62-bit value (0 .. 2^62 - 1)
 *
 * Surface (unique symbols):
 *   int gj_quic_varint_encode(uint64_t v, unsigned char *out, size_t cap,
 *                             size_t *n);
 *     — encode v with the shortest legal encoding into out[0..cap).
 *       *n = bytes written. Returns 0 / -1 (NULL args, v >= 2^62, or
 *       cap too small).
 *   int gj_quic_varint_decode(const unsigned char *in, size_t len,
 *                             uint64_t *v, size_t *n);
 *     — decode one varint from in[0..len). *v = value, *n = bytes
 *       consumed. Returns 0 / -1 (NULL args or truncated input).
 *   __gj_quic_varint_encode / __gj_quic_varint_decode  (aliases)
 *   __libcgj_batch200_marker = "libcgj-batch200"
 *
 * Notes:
 *   - Distinct from protobuf LEB128 varints (batch131 / batch166).
 *   - Max encode size is 8 bytes; max value is 2^62 - 1.
 *   - No errno (freestanding-safe); callers use return codes only.
 *   - Encoders always use the shortest form; decoders accept any legal
 *     length prefix (including over-long encodings within the 62-bit
 *     range, per the wire grammar).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch200_marker[] = "libcgj-batch200";

/* Largest value encodable as a QUIC varint (2^62 - 1). */
#define B200_MAX ((uint64_t)0x3fffffffffffffffull)

/* ---- encode ------------------------------------------------------------ */

/*
 * Encode v as a QUIC variable-length integer into out[0..cap).
 * Picks the shortest encoding that can hold v.
 * On success *pn is the byte count written (1, 2, 4, or 8).
 * Returns 0 on success, -1 if out/n is NULL, v exceeds 2^62-1, or
 * cap is smaller than the needed encoding length.
 */
int
gj_quic_varint_encode(uint64_t v, unsigned char *out, size_t cap, size_t *n)
{
	size_t need;
	unsigned char prefix;

	if (out == NULL || n == NULL) {
		return -1;
	}
	if (v > B200_MAX) {
		return -1;
	}

	if (v < (1ull << 6)) {
		need = 1u;
		prefix = 0x00u;
	} else if (v < (1ull << 14)) {
		need = 2u;
		prefix = 0x40u;
	} else if (v < (1ull << 30)) {
		need = 4u;
		prefix = 0x80u;
	} else {
		need = 8u;
		prefix = 0xc0u;
	}

	if (cap < need) {
		return -1;
	}

	/*
	 * Write value big-endian into need bytes, then OR the 2-bit length
	 * prefix into the high bits of the first byte. The value never
	 * sets those two bits for a shortest-form encode (they lie above
	 * the significance of each range), so OR is sufficient.
	 */
	{
		size_t i;
		uint64_t u = v;

		for (i = need; i > 0u; i--) {
			out[i - 1u] = (unsigned char)(u & 0xffu);
			u >>= 8;
		}
		out[0] = (unsigned char)(out[0] | prefix);
	}

	*n = need;
	return 0;
}

int __gj_quic_varint_encode(uint64_t v, unsigned char *out, size_t cap,
                            size_t *n)
    __attribute__((alias("gj_quic_varint_encode")));

/* ---- decode ------------------------------------------------------------ */

/*
 * Decode one QUIC variable-length integer from in[0..len).
 * Length is taken from the two MSBs of in[0]; the remaining bits of the
 * first byte plus subsequent bytes form a big-endian integer.
 * On success *pv is the value and *pn is the number of bytes consumed.
 * Returns 0 on success, -1 if in/v/n is NULL or the buffer is truncated.
 */
int
gj_quic_varint_decode(const unsigned char *in, size_t len, uint64_t *v,
                      size_t *n)
{
	unsigned char b0;
	size_t need;
	uint64_t uVal;
	size_t i;

	if (in == NULL || v == NULL || n == NULL) {
		return -1;
	}
	if (len == 0u) {
		return -1;
	}

	b0 = in[0];
	switch (b0 >> 6) {
	case 0:
		need = 1u;
		break;
	case 1:
		need = 2u;
		break;
	case 2:
		need = 4u;
		break;
	default:
		need = 8u;
		break;
	}

	if (len < need) {
		return -1;
	}

	/* Strip length prefix bits; assemble remaining big-endian payload. */
	uVal = (uint64_t)(b0 & 0x3fu);
	for (i = 1u; i < need; i++) {
		uVal = (uVal << 8) | (uint64_t)in[i];
	}

	*v = uVal;
	*n = need;
	return 0;
}

int __gj_quic_varint_decode(const unsigned char *in, size_t len, uint64_t *v,
                            size_t *n)
    __attribute__((alias("gj_quic_varint_decode")));
