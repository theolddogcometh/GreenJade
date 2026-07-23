/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch166: ZigZag + protobuf-style signed varint
 * (sint64) encode helpers.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party protobuf source was copied.
 *
 * Surface (unique symbols):
 *   uint64_t gj_zigzag_encode(int64_t v);
 *     — Map signed int64 to unsigned so small |v| stay small on the wire:
 *       (v << 1) ^ (v >> 63) with arithmetic right-shift of the sign bit.
 *   int64_t  gj_zigzag_decode(uint64_t v);
 *     — Inverse: (v >> 1) ^ -(v & 1).
 *   int gj_pb_encode_sint64(int64_t v, unsigned char *out, size_t cap,
 *                           size_t *n);
 *     — ZigZag-encode v, then write as LEB128-style unsigned varint into
 *       out[0..cap). *n = bytes written. Returns 0 / -1.
 *   __gj_zigzag_encode / __gj_zigzag_decode / __gj_pb_encode_sint64
 *   __libcgj_batch166_marker = "libcgj-batch166"
 *
 * Notes:
 *   - Complements batch131 unsigned varint / tag helpers (no multi-def).
 *   - At most 10 bytes for a 64-bit varint payload.
 *   - No errno (freestanding-safe); callers use return codes only.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch166_marker[] = "libcgj-batch166";

/* Max encoded length of a 64-bit protobuf varint. */
#define B166_VARINT_MAX 10u

/* ---- ZigZag ------------------------------------------------------------- */

/*
 * Encode signed v as an unsigned ZigZag value.
 * Non-negative n → 2n; negative n → 2·|n|−1  (so −1 → 1, −2 → 3, …).
 * Right-shift of v must be arithmetic (sign-extending).
 */
uint64_t
gj_zigzag_encode(int64_t v)
{
	return ((uint64_t)v << 1) ^ (uint64_t)(v >> 63);
}

uint64_t __gj_zigzag_encode(int64_t v)
    __attribute__((alias("gj_zigzag_encode")));

/*
 * Decode ZigZag unsigned v back to signed int64.
 * Even codes → non-negative; odd codes → negative.
 */
int64_t
gj_zigzag_decode(uint64_t v)
{
	return (int64_t)((v >> 1) ^ (uint64_t)-(int64_t)(v & 1u));
}

int64_t __gj_zigzag_decode(uint64_t v)
    __attribute__((alias("gj_zigzag_decode")));

/* ---- protobuf sint64 (ZigZag + unsigned varint) ------------------------ */

/*
 * Encode signed v as protobuf sint64: ZigZag then LEB128-style varint.
 * Writes into out[0..cap); *pn = number of bytes produced.
 * Returns 0 on success, -1 if out/n is NULL or cap is too small.
 */
int
gj_pb_encode_sint64(int64_t v, unsigned char *out, size_t cap, size_t *n)
{
	uint64_t uVal;
	size_t i;

	if (out == NULL || n == NULL) {
		return -1;
	}

	uVal = gj_zigzag_encode(v);
	i = 0u;
	do {
		unsigned char b;

		if (i >= cap || i >= B166_VARINT_MAX) {
			return -1;
		}
		b = (unsigned char)(uVal & 0x7fu);
		uVal >>= 7;
		if (uVal != 0u) {
			b = (unsigned char)(b | 0x80u);
		}
		out[i] = b;
		i++;
	} while (uVal != 0u);

	*n = i;
	return 0;
}

int __gj_pb_encode_sint64(int64_t v, unsigned char *out, size_t cap, size_t *n)
    __attribute__((alias("gj_pb_encode_sint64")));
