/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch284: signed LEB128 encode/decode (int64).
 *
 * Surface (unique symbols — distinct from batch130 gj_sleb128_*):
 *   int gj_leb128s_encode(int64_t v, uint8_t *out, size_t cap,
 *                         size_t *nwritten);
 *   int gj_leb128s_decode(const uint8_t *in, size_t nin, int64_t *out,
 *                         size_t *nread);
 *   __gj_leb128s_encode / __gj_leb128s_decode  (aliases)
 *   __libcgj_batch284_marker = "libcgj-batch284"
 *
 * DWARF / WebAssembly SLEB128: 7 data bits per byte, MSB = continuation.
 * Encode stops when remaining high bits are all zero or all one and the
 * sign bit of the last group matches. Decode sign-extends from bit 6 of
 * the final group when total shift < 64.
 *
 * Return: 0 on success, -1 on error. On success *nwritten / *nread is the
 * byte count written or consumed. On error those outs are 0 when non-NULL.
 * Max encoded length for int64 is 10 bytes.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied. Does not redefine gj_sleb128_* (batch130).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch284_marker[] = "libcgj-batch284";

/* Max SLEB128 length for a 64-bit value: ceil(64/7) = 10. */
#define B284_LEB_MAX 10u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b284_sar7 — portable arithmetic right shift by 7 (sign-fill).
 * Avoids relying on implementation-defined signed >> of negative values.
 */
static int64_t
b284_sar7(int64_t i64V)
{
	uint64_t u64 = (uint64_t)i64V;

	u64 >>= 7;
	if (i64V < 0) {
		u64 |= (~(uint64_t)0ull) << (64u - 7u);
	}
	return (int64_t)u64;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_leb128s_encode — encode signed LEB128 of int64_t into out[0..cap).
 *
 * v:        value to encode (two's complement).
 * out:      destination buffer (must be non-NULL when cap is sufficient).
 * cap:      capacity of out in bytes.
 * nwritten: optional; set to encoded length on success, 0 on error.
 *
 * Writes nothing on error (uses a stack temp). Always encodes at least
 * one byte (0 → 0x00, -1 → 0x7f).
 */
int
gj_leb128s_encode(int64_t v, uint8_t *out, size_t cap, size_t *nwritten)
{
	uint8_t aTmp[B284_LEB_MAX];
	size_t cb = 0u;
	int64_t i64Rem = v;
	int fMore = 1;

	if (nwritten != NULL) {
		*nwritten = 0u;
	}

	while (fMore) {
		uint8_t u8B = (uint8_t)((uint64_t)i64Rem & 0x7full);

		i64Rem = b284_sar7(i64Rem);
		/*
		 * Stop when remaining is all 0 and group sign bit clear,
		 * or remaining is all 1s and group sign bit set.
		 */
		if ((i64Rem == 0 && (u8B & 0x40u) == 0u) ||
		    (i64Rem == (int64_t)-1 && (u8B & 0x40u) != 0u)) {
			fMore = 0;
		} else {
			u8B = (uint8_t)(u8B | 0x80u);
		}
		if (cb >= B284_LEB_MAX) {
			return -1;
		}
		aTmp[cb++] = u8B;
	}

	if (cap < cb || out == NULL) {
		return -1;
	}
	{
		size_t iByte;

		for (iByte = 0u; iByte < cb; iByte++) {
			out[iByte] = aTmp[iByte];
		}
	}
	if (nwritten != NULL) {
		*nwritten = cb;
	}
	return 0;
}

/*
 * gj_leb128s_decode — decode signed LEB128 from in[0..nin) into *out.
 *
 * in:    source buffer.
 * nin:   available input length.
 * out:   destination for decoded int64_t.
 * nread: optional; set to bytes consumed on success, 0 on error.
 *
 * Rejects truncated streams, overlong encodings (>10 bytes), and
 * continuation past a filled 64-bit value with non-sign-compatible groups.
 */
int
gj_leb128s_decode(const uint8_t *in, size_t nin, int64_t *out, size_t *nread)
{
	uint64_t u64Val = 0u;
	unsigned uShift = 0u;
	size_t iByte = 0u;
	uint8_t u8B = 0u;

	if (nread != NULL) {
		*nread = 0u;
	}
	if (in == NULL || out == NULL || nin == 0u) {
		return -1;
	}

	for (;;) {
		if (iByte >= nin || iByte >= B284_LEB_MAX) {
			return -1;
		}
		u8B = in[iByte++];
		if (uShift < 64u) {
			u64Val |= ((uint64_t)(u8B & 0x7fu)) << uShift;
			uShift += 7u;
		} else {
			/* Already filled 64 bits; only a terminator allowed. */
			if ((u8B & 0x7fu) != 0u && (u8B & 0x7fu) != 0x7fu) {
				return -1;
			}
			if ((u8B & 0x80u) != 0u) {
				return -1;
			}
			break;
		}
		if ((u8B & 0x80u) == 0u) {
			break;
		}
	}

	/* Sign-extend if last group had bit 6 set and shift < 64. */
	if (uShift < 64u && (u8B & 0x40u) != 0u) {
		u64Val |= (~(uint64_t)0ull) << uShift;
	}

	*out = (int64_t)u64Val;
	if (nread != NULL) {
		*nread = iByte;
	}
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_leb128s_encode(int64_t v, uint8_t *out, size_t cap, size_t *nwritten)
    __attribute__((alias("gj_leb128s_encode")));
int __gj_leb128s_decode(const uint8_t *in, size_t nin, int64_t *out,
                        size_t *nread)
    __attribute__((alias("gj_leb128s_decode")));
