/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch130: LEB128 encode/decode (unsigned + signed).
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party LEB128 source was copied.
 *
 * DWARF / WebAssembly style variable-length integers:
 *   - ULEB128: 7 data bits per byte, MSB = continuation
 *   - SLEB128: same packing; encode stops when remaining high bits are
 *     all zero or all one and the sign bit of the last group matches
 *
 * Surface (unique symbols):
 *   int gj_uleb128_encode(uint64_t v, unsigned char *out, size_t outcap,
 *                         size_t *n);
 *   int gj_uleb128_decode(const unsigned char *in, size_t len, uint64_t *v,
 *                         size_t *n);
 *   int gj_sleb128_encode(int64_t v, unsigned char *out, size_t outcap,
 *                         size_t *n);
 *   int gj_sleb128_decode(const unsigned char *in, size_t len, int64_t *v,
 *                         size_t *n);
 *   __gj_uleb128_encode / __gj_uleb128_decode
 *   __gj_sleb128_encode / __gj_sleb128_decode  (aliases)
 *   __libcgj_batch130_marker = "libcgj-batch130"
 *
 * Return: 0 on success, -1 on error. On success *n is the byte count
 * written or consumed. On error *n is 0 when non-NULL (when set).
 * Max encoded length for 64-bit values is 10 bytes.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch130_marker[] = "libcgj-batch130";

/* Max LEB128 length for a 64-bit value: ceil(64/7) = 10. */
#define B130_LEB_MAX 10u

/* ---- ULEB128 ------------------------------------------------------------ */

/*
 * Encode unsigned LEB128. out may be NULL only when outcap == 0 to query
 * length; if outcap is insufficient, returns -1 without partial write.
 */
int
gj_uleb128_encode(uint64_t v, unsigned char *out, size_t outcap, size_t *n)
{
	unsigned char aTmp[B130_LEB_MAX];
	size_t cb = 0u;
	uint64_t u = v;

	/* Always at least one byte (zero encodes as 0x00). */
	for (;;) {
		unsigned char b = (unsigned char)(u & 0x7fu);

		u >>= 7;
		if (u != 0u) {
			b = (unsigned char)(b | 0x80u);
		}
		aTmp[cb++] = b;
		if (u == 0u) {
			break;
		}
	}

	if (n != NULL) {
		*n = 0u;
	}
	if (outcap < cb) {
		return -1;
	}
	if (out == NULL) {
		/* Need room reported above; NULL out only ok if cb==0 (never). */
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

/*
 * Decode unsigned LEB128. Rejects overlong streams beyond 10 bytes and
 * truncated input (continuation set with no following byte).
 */
int
gj_uleb128_decode(const unsigned char *in, size_t len, uint64_t *v, size_t *n)
{
	uint64_t uVal = 0u;
	unsigned uShift = 0u;
	size_t i = 0u;

	if (n != NULL) {
		*n = 0u;
	}
	if (in == NULL || v == NULL || len == 0u) {
		return -1;
	}

	for (;;) {
		unsigned char b;

		if (i >= len || i >= B130_LEB_MAX) {
			return -1;
		}
		b = in[i++];
		/*
		 * Low 7 bits contribute. After shift >= 64 nothing more
		 * may be non-zero (overflow); allow a final zero group
		 * only if it terminates.
		 */
		if (uShift == 63u) {
			/* Only bit 0 of the group may be set for uint64. */
			if ((b & 0x7eu) != 0u) {
				return -1;
			}
			uVal |= ((uint64_t)(b & 0x01u)) << 63;
			if ((b & 0x80u) != 0u) {
				return -1;
			}
			break;
		}
		uVal |= ((uint64_t)(b & 0x7fu)) << uShift;
		if ((b & 0x80u) == 0u) {
			break;
		}
		uShift += 7u;
	}

	*v = uVal;
	if (n != NULL) {
		*n = i;
	}
	return 0;
}

/* ---- SLEB128 ------------------------------------------------------------ */

/*
 * Encode signed LEB128 (two's complement). Terminates when the remaining
 * value is 0 with sign bit clear in the last group, or all ones with
 * sign bit set.
 */
int
gj_sleb128_encode(int64_t v, unsigned char *out, size_t outcap, size_t *n)
{
	unsigned char aTmp[B130_LEB_MAX];
	size_t cb = 0u;
	int64_t s = v;
	int bMore = 1;

	while (bMore) {
		unsigned char b = (unsigned char)(s & 0x7f);
		/* Arithmetic right shift of signed value. */
		s >>= 7;
		/*
		 * Stop when remaining is all 0 and sign bit of group is 0,
		 * or remaining is all 1s and sign bit of group is 1.
		 */
		if ((s == 0 && (b & 0x40u) == 0u) ||
		    (s == (int64_t)-1 && (b & 0x40u) != 0u)) {
			bMore = 0;
		} else {
			b = (unsigned char)(b | 0x80u);
		}
		aTmp[cb++] = b;
		if (cb > B130_LEB_MAX) {
			if (n != NULL) {
				*n = 0u;
			}
			return -1;
		}
	}

	if (n != NULL) {
		*n = 0u;
	}
	if (outcap < cb || out == NULL) {
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

/*
 * Decode signed LEB128. Sign-extends from the last group's bit 6 when
 * the total shift is less than 64.
 */
int
gj_sleb128_decode(const unsigned char *in, size_t len, int64_t *v, size_t *n)
{
	uint64_t uVal = 0u;
	unsigned uShift = 0u;
	size_t i = 0u;
	unsigned char b = 0u;

	if (n != NULL) {
		*n = 0u;
	}
	if (in == NULL || v == NULL || len == 0u) {
		return -1;
	}

	for (;;) {
		if (i >= len || i >= B130_LEB_MAX) {
			return -1;
		}
		b = in[i++];
		if (uShift < 64u) {
			uVal |= ((uint64_t)(b & 0x7fu)) << uShift;
			uShift += 7u;
		} else {
			/* Already filled 64 bits; only terminator allowed. */
			if ((b & 0x7fu) != 0u && (b & 0x7fu) != 0x7fu) {
				return -1;
			}
			if ((b & 0x80u) != 0u) {
				return -1;
			}
			break;
		}
		if ((b & 0x80u) == 0u) {
			break;
		}
	}

	/* Sign-extend if the last group had bit 6 set and shift < 64. */
	if (uShift < 64u && (b & 0x40u) != 0u) {
		uVal |= (~(uint64_t)0u) << uShift;
	}

	*v = (int64_t)uVal;
	if (n != NULL) {
		*n = i;
	}
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_uleb128_encode(uint64_t v, unsigned char *out, size_t outcap, size_t *n)
    __attribute__((alias("gj_uleb128_encode")));
int __gj_uleb128_decode(const unsigned char *in, size_t len, uint64_t *v,
                        size_t *n)
    __attribute__((alias("gj_uleb128_decode")));
int __gj_sleb128_encode(int64_t v, unsigned char *out, size_t outcap, size_t *n)
    __attribute__((alias("gj_sleb128_encode")));
int __gj_sleb128_decode(const unsigned char *in, size_t len, int64_t *v,
                        size_t *n)
    __attribute__((alias("gj_sleb128_decode")));
