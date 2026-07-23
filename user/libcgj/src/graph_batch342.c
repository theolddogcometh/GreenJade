/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch342: QUIC variable-length integer encode/decode
 * (RFC 9000 §16) under distinct symbols from batch200.
 *
 * Clean-room freestanding pure C — integer/pointer only. Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc helpers. No third-party QUIC source was copied.
 *
 * Wire format (network / big-endian; length encoded in the two MSBs of
 * the first byte):
 *   00xxxxxx                → 1 byte,  6-bit value  (0 .. 2^6  - 1)
 *   01xxxxxx xxxxxxxx       → 2 bytes, 14-bit value (0 .. 2^14 - 1)
 *   10xxxxxx ... (4 total)  → 4 bytes, 30-bit value (0 .. 2^30 - 1)
 *   11xxxxxx ... (8 total)  → 8 bytes, 62-bit value (0 .. 2^62 - 1)
 *
 * Surface (unique symbols — do NOT collide with batch200 gj_quic_varint_*):
 *   int gj_quic_vi_encode(uint64_t v, uint8_t *out, size_t cap, size_t *n);
 *     — encode v with the shortest legal encoding into out[0..cap).
 *       *n = bytes written. Returns 0 / -1 (NULL args, v >= 2^62, or
 *       cap too small).
 *   int gj_quic_vi_decode(const uint8_t *in, size_t nin, uint64_t *v,
 *                         size_t *nread);
 *     — decode one varint from in[0..nin). *v = value, *nread = bytes
 *       consumed. Returns 0 / -1 (NULL args or truncated input).
 *   __gj_quic_vi_encode / __gj_quic_vi_decode  (aliases)
 *   __libcgj_batch342_marker = "libcgj-batch342"
 *
 * Notes:
 *   - Distinct from gj_quic_varint_* (batch200) and protobuf LEB128
 *     (batch131 / batch166 / batch283).
 *   - Max encode size is 8 bytes; max value is 2^62 - 1.
 *   - Encoders always use the shortest form; decoders accept any legal
 *     length prefix (including over-long encodings within the 62-bit
 *     range, per the wire grammar).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch342_marker[] = "libcgj-batch342";

/* Largest value encodable as a QUIC varint (2^62 - 1). */
#define B342_MAX ((uint64_t)0x3fffffffffffffffull)

/* ---- encode ------------------------------------------------------------ */

/*
 * Encode v as a QUIC variable-length integer into out[0..cap).
 * Picks the shortest encoding that can hold v.
 * On success *pn is the byte count written (1, 2, 4, or 8).
 * Returns 0 on success, -1 if out/n is NULL, v exceeds 2^62-1, or
 * cap is smaller than the needed encoding length.
 */
int
gj_quic_vi_encode(uint64_t v, uint8_t *out, size_t cap, size_t *n)
{
	size_t cbNeed;
	uint8_t u8Prefix;
	size_t iByte;
	uint64_t u64Val;

	if (out == NULL || n == NULL) {
		return -1;
	}
	if (v > B342_MAX) {
		return -1;
	}

	if (v < (1ull << 6)) {
		cbNeed = 1u;
		u8Prefix = 0x00u;
	} else if (v < (1ull << 14)) {
		cbNeed = 2u;
		u8Prefix = 0x40u;
	} else if (v < (1ull << 30)) {
		cbNeed = 4u;
		u8Prefix = 0x80u;
	} else {
		cbNeed = 8u;
		u8Prefix = 0xc0u;
	}

	if (cap < cbNeed) {
		return -1;
	}

	/*
	 * Write value big-endian into cbNeed bytes, then OR the 2-bit length
	 * prefix into the high bits of the first byte. The value never sets
	 * those two bits for a shortest-form encode (they lie above the
	 * significance of each range), so OR is sufficient.
	 */
	u64Val = v;
	for (iByte = cbNeed; iByte > 0u; iByte--) {
		out[iByte - 1u] = (uint8_t)(u64Val & 0xffu);
		u64Val >>= 8;
	}
	out[0] = (uint8_t)(out[0] | u8Prefix);

	*n = cbNeed;
	return 0;
}

int __gj_quic_vi_encode(uint64_t v, uint8_t *out, size_t cap, size_t *n)
    __attribute__((alias("gj_quic_vi_encode")));

/* ---- decode ------------------------------------------------------------ */

/*
 * Decode one QUIC variable-length integer from in[0..nin).
 * Length is taken from the two MSBs of in[0]; the remaining bits of the
 * first byte plus subsequent bytes form a big-endian integer.
 * On success *pv is the value and *pnread is the number of bytes consumed.
 * Returns 0 on success, -1 if in/v/nread is NULL or the buffer is truncated.
 */
int
gj_quic_vi_decode(const uint8_t *in, size_t nin, uint64_t *v, size_t *nread)
{
	uint8_t u8B0;
	size_t cbNeed;
	uint64_t u64Val;
	size_t iByte;

	if (in == NULL || v == NULL || nread == NULL) {
		return -1;
	}
	if (nin == 0u) {
		return -1;
	}

	u8B0 = in[0];
	switch (u8B0 >> 6) {
	case 0:
		cbNeed = 1u;
		break;
	case 1:
		cbNeed = 2u;
		break;
	case 2:
		cbNeed = 4u;
		break;
	default:
		cbNeed = 8u;
		break;
	}

	if (nin < cbNeed) {
		return -1;
	}

	/* Strip length prefix bits; assemble remaining big-endian payload. */
	u64Val = (uint64_t)(u8B0 & 0x3fu);
	for (iByte = 1u; iByte < cbNeed; iByte++) {
		u64Val = (u64Val << 8) | (uint64_t)in[iByte];
	}

	*v = u64Val;
	*nread = cbNeed;
	return 0;
}

int __gj_quic_vi_decode(const uint8_t *in, size_t nin, uint64_t *v,
                        size_t *nread)
    __attribute__((alias("gj_quic_vi_decode")));
