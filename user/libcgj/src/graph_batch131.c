/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch131: Protocol Buffers wire helpers —
 * unsigned varint and key (tag) encode/decode.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party protobuf source was copied.
 *
 * Surface (0 success, -1 error with errno):
 *   int gj_pb_encode_varint(uint64_t v, unsigned char *out, size_t outcap,
 *                           size_t *n);
 *     Encode unsigned LEB128-style protobuf varint. *n = bytes written.
 *   int gj_pb_decode_varint(const unsigned char *in, size_t len,
 *                           uint64_t *v, size_t *n);
 *     Decode varint from in[0..len). *n = bytes consumed; *v = value.
 *   int gj_pb_encode_tag(unsigned field, unsigned wire, unsigned char *out,
 *                        size_t outcap, size_t *n);
 *     Encode key = (field << 3) | (wire & 7) as a varint.
 *   int gj_pb_decode_tag(const unsigned char *in, size_t len,
 *                        unsigned *field, unsigned *wire, size_t *n);
 *     Decode key: field = tag >> 3, wire = tag & 7.
 *   __gj_pb_encode_varint / __gj_pb_decode_varint
 *   __gj_pb_encode_tag / __gj_pb_decode_tag  (aliases)
 *   __libcgj_batch131_marker = "libcgj-batch131"
 *
 * Limits (protobuf wire): at most 10 bytes per 64-bit varint; field
 * number must be non-zero; wire type is the low 3 bits of the key.
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch131_marker[] = "libcgj-batch131";

/* Max encoded length of a 64-bit unsigned protobuf varint. */
#define B131_VARINT_MAX 10u

/* ---- varint encode ----------------------------------------------------- */

/*
 * Encode v as a protobuf unsigned varint into out[0..outcap).
 * Returns 0 and *pn = length on success; -1 with errno on error.
 */
int
gj_pb_encode_varint(uint64_t v, unsigned char *out, size_t outcap, size_t *n)
{
	uint64_t uVal;
	size_t i;

	if (out == NULL || n == NULL) {
		errno = EINVAL;
		return -1;
	}

	uVal = v;
	i = 0u;
	do {
		unsigned char b;

		if (i >= outcap) {
			errno = ENOSPC;
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

/* ---- varint decode ----------------------------------------------------- */

/*
 * Decode a protobuf unsigned varint from in[0..len).
 * Rejects truncated streams, overlong encodings (>10 bytes), and
 * continuation bits that would shift past 64 bits of significance.
 */
int
gj_pb_decode_varint(const unsigned char *in, size_t len, uint64_t *v,
                    size_t *n)
{
	uint64_t uVal;
	unsigned uShift;
	size_t i;

	if (in == NULL || v == NULL || n == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (len == 0u) {
		errno = EINVAL;
		return -1;
	}

	uVal = 0u;
	uShift = 0u;
	for (i = 0u; i < len && i < B131_VARINT_MAX; i++) {
		unsigned char b = in[i];
		uint64_t uChunk = (uint64_t)(b & 0x7fu);

		/*
		 * On the 10th byte only the low bit may be set (64-bit
		 * payload uses 9*7 + 1 = 64 bits). Higher bits would
		 * overflow uint64_t.
		 */
		if (uShift == 63u) {
			if (uChunk > 1u) {
				errno = EINVAL;
				return -1;
			}
		} else if (uShift > 63u) {
			errno = EINVAL;
			return -1;
		}

		uVal |= uChunk << uShift;
		if ((b & 0x80u) == 0u) {
			*v = uVal;
			*n = i + 1u;
			return 0;
		}
		uShift += 7u;
	}

	/* Truncated, or still continuing after max length. */
	errno = EINVAL;
	return -1;
}

/* ---- tag (key) encode -------------------------------------------------- */

/*
 * Protobuf key: (field_number << 3) | wire_type.
 * field must be non-zero; wire is masked to 3 bits (0..7).
 */
int
gj_pb_encode_tag(unsigned field, unsigned wire, unsigned char *out,
                 size_t outcap, size_t *n)
{
	uint64_t uTag;

	if (field == 0u) {
		errno = EINVAL;
		return -1;
	}
	/* field fits in remaining 61 bits of a 64-bit key after wire. */
	if (field > (unsigned)(UINT64_MAX >> 3)) {
		errno = EINVAL;
		return -1;
	}

	uTag = ((uint64_t)field << 3) | (uint64_t)(wire & 7u);
	return gj_pb_encode_varint(uTag, out, outcap, n);
}

/* ---- tag (key) decode -------------------------------------------------- */

/*
 * Decode key varint into field number and wire type.
 * field == 0 is rejected (invalid per protobuf).
 */
int
gj_pb_decode_tag(const unsigned char *in, size_t len, unsigned *field,
                 unsigned *wire, size_t *n)
{
	uint64_t uTag;
	size_t cb;
	uint64_t uField;

	if (field == NULL || wire == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (gj_pb_decode_varint(in, len, &uTag, &cb) != 0) {
		return -1;
	}

	uField = uTag >> 3;
	/* Fit into unsigned without silent truncation of huge tags. */
	if (uField > (uint64_t)(unsigned)-1) {
		errno = EINVAL;
		return -1;
	}
	if (uField == 0u) {
		errno = EINVAL;
		return -1;
	}

	*field = (unsigned)uField;
	*wire = (unsigned)(uTag & 7u);
	if (n != NULL) {
		*n = cb;
	}
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_pb_encode_varint(uint64_t v, unsigned char *out, size_t outcap,
                          size_t *n)
    __attribute__((alias("gj_pb_encode_varint")));
int __gj_pb_decode_varint(const unsigned char *in, size_t len, uint64_t *v,
                          size_t *n)
    __attribute__((alias("gj_pb_decode_varint")));
int __gj_pb_encode_tag(unsigned field, unsigned wire, unsigned char *out,
                       size_t outcap, size_t *n)
    __attribute__((alias("gj_pb_encode_tag")));
int __gj_pb_decode_tag(const unsigned char *in, size_t len, unsigned *field,
                       unsigned *wire, size_t *n)
    __attribute__((alias("gj_pb_decode_tag")));
