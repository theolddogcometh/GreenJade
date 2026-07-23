/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch135: HTTP/2 HPACK integer coding (RFC 7541 §5.1).
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party HPACK source was copied.
 *
 * Prefix-N integer representation used throughout HPACK (indexed header
 * fields, literal name/value lengths, dynamic table size updates, etc.):
 *   - N is the number of bits in the first-byte prefix (1..8).
 *   - If value < 2^N - 1, it is stored in those N bits alone.
 *   - Otherwise the N bits are all ones and the remainder is encoded as
 *     a base-128 integer (7 data bits per byte, MSB = continuation).
 *   Bits above the N-bit prefix in the first byte are not part of the
 *   integer (flags). Encode leaves them zero; decode ignores them.
 *
 * Surface (unique symbols):
 *   int gj_hpack_encode_int(unsigned char *out, size_t outcap, unsigned N,
 *                           uint32_t value, size_t *n);
 *     Encode value with an N-bit prefix into out[0..outcap).
 *     *n = bytes written on success.
 *   int gj_hpack_decode_int(const unsigned char *in, size_t len, unsigned N,
 *                           uint32_t *value, size_t *n);
 *     Decode a prefix-N integer from in[0..len). *value = result;
 *     *n = bytes consumed on success.
 *   __gj_hpack_encode_int / __gj_hpack_decode_int  (aliases)
 *   __libcgj_batch135_marker = "libcgj-batch135"
 *
 * Return: 0 on success, -1 on error. On error *n is 0 when non-NULL.
 * Max encoded length for a uint32_t value is 6 bytes (1 prefix + 5 cont).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch135_marker[] = "libcgj-batch135";

/*
 * Worst case: N=1, value = UINT32_MAX → remainder needs ceil(32/7)=5
 * continuation bytes plus the first byte → 6.
 */
#define B135_HPACK_INT_MAX 6u

/* ---- encode ------------------------------------------------------------ */

/*
 * Encode HPACK integer (RFC 7541 §5.1) with an N-bit prefix.
 * Requires 1 <= N <= 8. out must be non-NULL; outcap must hold the full
 * encoding (no partial writes). *n is optional.
 */
int
gj_hpack_encode_int(unsigned char *out, size_t outcap, unsigned N,
                    uint32_t value, size_t *n)
{
	unsigned char aTmp[B135_HPACK_INT_MAX];
	size_t cb;
	uint32_t uMax;
	uint32_t uRem;

	if (n != NULL) {
		*n = 0u;
	}
	if (out == NULL || N < 1u || N > 8u) {
		return -1;
	}

	uMax = ((uint32_t)1u << N) - 1u;

	if (value < uMax) {
		/* Fits in the N-bit prefix alone. High bits of first byte = 0. */
		aTmp[0] = (unsigned char)(value & uMax);
		cb = 1u;
	} else {
		/*
		 * Prefix is all ones; remainder (value - max) is base-128 with
		 * continuation bit 0x80 on every non-final byte.
		 */
		aTmp[0] = (unsigned char)uMax;
		uRem = value - uMax;
		cb = 1u;
		while (uRem >= 128u) {
			if (cb >= B135_HPACK_INT_MAX) {
				return -1;
			}
			aTmp[cb++] = (unsigned char)((uRem & 0x7fu) | 0x80u);
			uRem >>= 7;
		}
		if (cb >= B135_HPACK_INT_MAX) {
			return -1;
		}
		aTmp[cb++] = (unsigned char)(uRem & 0x7fu);
	}

	if (outcap < cb) {
		return -1;
	}
	{
		size_t i;

		for (i = 0u; i < cb; i++) {
			out[i] = aTmp[i];
		}
	}
	if (n != NULL) {
		*n = cb;
	}
	return 0;
}

/* ---- decode ------------------------------------------------------------ */

/*
 * Decode HPACK integer (RFC 7541 §5.1) with an N-bit prefix.
 * Ignores bits above the N-bit prefix in the first byte (flags).
 * Rejects truncated streams and values that would overflow uint32_t.
 */
int
gj_hpack_decode_int(const unsigned char *in, size_t len, unsigned N,
                    uint32_t *value, size_t *n)
{
	uint32_t uMax;
	uint32_t uVal;
	size_t i;

	if (n != NULL) {
		*n = 0u;
	}
	if (in == NULL || value == NULL || len == 0u || N < 1u || N > 8u) {
		return -1;
	}

	uMax = ((uint32_t)1u << N) - 1u;
	uVal = (uint32_t)in[0] & uMax;
	i = 1u;

	if (uVal < uMax) {
		/* Single-byte encoding. */
		*value = uVal;
		if (n != NULL) {
			*n = i;
		}
		return 0;
	}

	/*
	 * Multi-byte: I = max + sum over k of (b_k & 0x7f) * 2^(7*k)
	 * until a byte with MSB clear. Cap total length at B135_HPACK_INT_MAX
	 * and refuse arithmetic that would exceed UINT32_MAX.
	 */
	{
		unsigned uShift = 0u;

		for (;;) {
			unsigned char b;
			uint32_t uChunk;

			if (i >= len || i >= B135_HPACK_INT_MAX) {
				return -1;
			}
			b = in[i++];
			uChunk = (uint32_t)(b & 0x7fu);

			/*
			 * Add uChunk << uShift to uVal without wrapping.
			 * When uShift >= 32 the only legal contribution is 0
			 * (and that byte must be the terminator).
			 */
			if (uShift >= 32u) {
				if (uChunk != 0u) {
					return -1;
				}
			} else if (uChunk != 0u) {
				uint32_t uAdd;

				/* Reject shift that would lose bits of uChunk. */
				if (uChunk > (UINT32_MAX >> uShift)) {
					return -1;
				}
				uAdd = uChunk << uShift;
				if (uVal > UINT32_MAX - uAdd) {
					return -1;
				}
				uVal += uAdd;
			}

			if ((b & 0x80u) == 0u) {
				break;
			}
			uShift += 7u;
			/* Continuation past 32-bit range is impossible to finish. */
			if (uShift > 32u) {
				return -1;
			}
		}
	}

	*value = uVal;
	if (n != NULL) {
		*n = i;
	}
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_hpack_encode_int(unsigned char *out, size_t outcap, unsigned N,
                          uint32_t value, size_t *n)
    __attribute__((alias("gj_hpack_encode_int")));
int __gj_hpack_decode_int(const unsigned char *in, size_t len, unsigned N,
                          uint32_t *value, size_t *n)
    __attribute__((alias("gj_hpack_decode_int")));
