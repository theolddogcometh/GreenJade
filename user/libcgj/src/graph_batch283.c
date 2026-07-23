/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch283: protobuf-style unsigned varint
 * encode/decode (LEB128 wire packing).
 *
 * Surface (unique symbols — do NOT collide with batch130/131/200 names):
 *   int gj_varint_encode(uint64_t v, uint8_t *out, size_t cap,
 *                        size_t *nwritten);
 *     — Encode v as an unsigned protobuf varint (7 data bits per byte,
 *       MSB = continuation) into out[0..cap). *nwritten = bytes written.
 *       Zero encodes as a single 0x00. At most 10 bytes for uint64_t.
 *       Returns 0 on success, -1 on error (NULL out/nwritten or short cap).
 *   int gj_varint_decode(const uint8_t *in, size_t nin, uint64_t *out,
 *                        size_t *nread);
 *     — Decode one unsigned varint from in[0..nin). *out = value,
 *       *nread = bytes consumed. Rejects truncated streams, overlong
 *       encodings (>10 B), and overflow past 64 bits of significance.
 *       Returns 0 on success, -1 on error.
 *   __gj_varint_encode / __gj_varint_decode  (aliases)
 *   __libcgj_batch283_marker = "libcgj-batch283"
 *
 * Distinct from:
 *   gj_uleb128_*          → graph_batch130.c
 *   gj_pb_encode_varint   → graph_batch131.c
 *   gj_quic_varint_*      → graph_batch200.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party protobuf source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch283_marker[] = "libcgj-batch283";

/* Max encoded length of a 64-bit unsigned protobuf varint: ceil(64/7)=10. */
#define B283_VARINT_MAX 10u

/* ---- encode ------------------------------------------------------------ */

/*
 * gj_varint_encode — protobuf unsigned varint into out[0..cap).
 *
 * Wire: each byte carries 7 payload bits in the low bits; the high bit
 * is set while more bytes follow. Canonical (shortest) form only.
 *
 * Edge cases:
 *   out == NULL or nwritten == NULL → -1
 *   cap too small for the encoding   → -1 (no partial write guarantee
 *                                      beyond bytes already written in
 *                                      the loop; capacity is checked
 *                                      before each store)
 * Success: *nwritten in 1..10, return 0.
 */
int
gj_varint_encode(uint64_t v, uint8_t *out, size_t cap, size_t *nwritten)
{
	uint64_t u64Val;
	size_t iByte;

	if (out == NULL || nwritten == NULL) {
		return -1;
	}

	u64Val = v;
	iByte = 0u;
	do {
		uint8_t u8B;

		if (iByte >= cap) {
			return -1;
		}
		u8B = (uint8_t)(u64Val & 0x7fu);
		u64Val >>= 7;
		if (u64Val != 0u) {
			u8B = (uint8_t)(u8B | 0x80u);
		}
		out[iByte] = u8B;
		iByte++;
	} while (u64Val != 0u);

	*nwritten = iByte;
	return 0;
}

int __gj_varint_encode(uint64_t v, uint8_t *out, size_t cap, size_t *nwritten)
    __attribute__((alias("gj_varint_encode")));

/* ---- decode ------------------------------------------------------------ */

/*
 * gj_varint_decode — parse one unsigned protobuf varint from in[0..nin).
 *
 * Rejects:
 *   NULL in/out/nread
 *   empty buffer (nin == 0)
 *   truncated continuation (MSB set with no following byte)
 *   encodings longer than B283_VARINT_MAX
 *   a 10th byte with more than the single low bit set (would overflow
 *     uint64_t: 9*7 + 1 = 64 payload bits)
 *
 * On success *out holds the value and *nread the byte count consumed.
 */
int
gj_varint_decode(const uint8_t *in, size_t nin, uint64_t *out, size_t *nread)
{
	uint64_t u64Val;
	unsigned uShift;
	size_t iByte;

	if (in == NULL || out == NULL || nread == NULL) {
		return -1;
	}
	if (nin == 0u) {
		return -1;
	}

	u64Val = 0u;
	uShift = 0u;
	for (iByte = 0u; iByte < nin && iByte < (size_t)B283_VARINT_MAX;
	     iByte++) {
		uint8_t u8B = in[iByte];
		uint64_t u64Chunk = (uint64_t)(u8B & 0x7fu);

		/*
		 * After 9 full groups (63 bits), only bit 0 of the 10th
		 * group may contribute. Higher bits or a further
		 * continuation are invalid for a 64-bit value.
		 */
		if (uShift == 63u) {
			if (u64Chunk > 1u) {
				return -1;
			}
			if ((u8B & 0x80u) != 0u) {
				return -1;
			}
			u64Val |= u64Chunk << 63;
			*out = u64Val;
			*nread = iByte + 1u;
			return 0;
		}

		u64Val |= u64Chunk << uShift;
		if ((u8B & 0x80u) == 0u) {
			*out = u64Val;
			*nread = iByte + 1u;
			return 0;
		}
		uShift += 7u;
	}

	/* Truncated, or still continuing after the max legal length. */
	return -1;
}

int __gj_varint_decode(const uint8_t *in, size_t nin, uint64_t *out,
                       size_t *nread)
    __attribute__((alias("gj_varint_decode")));
