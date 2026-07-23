/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch127: MQTT 3.1.1 fixed header + Remaining Length
 * (OASIS MQTT Version 3.1.1, section 2.2).
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party MQTT source was copied.
 *
 * Surface (0 success, -1 error):
 *   int gj_mqtt_encode_remaining(unsigned char *out, size_t outcap,
 *                                uint32_t rem, size_t *n);
 *     Encode rem as MQTT Remaining Length into out[0..). Writes minimal
 *     1..4 byte form. *n = encoded byte count. rem must be ≤ 268435455.
 *   int gj_mqtt_decode_remaining(const unsigned char *in, size_t len,
 *                                uint32_t *rem, size_t *n);
 *     Decode Remaining Length from in[0..len). *rem = value, *n = bytes
 *     consumed (1..4). Rejects truncated / >4-byte / out-of-range encodings.
 *   int gj_mqtt_fixed_header(const unsigned char *in, size_t len,
 *                            int *type, int *flags, uint32_t *rem_len,
 *                            size_t *hdr_len);
 *     Parse MQTT fixed header: first byte type (high nibble) + flags (low
 *     nibble), then Remaining Length. *hdr_len = 1 + RL byte count.
 *   __gj_mqtt_encode_remaining / __gj_mqtt_decode_remaining /
 *   __gj_mqtt_fixed_header  (aliases)
 *   __libcgj_batch127_marker = "libcgj-batch127"
 *
 * MQTT fixed header layout:
 *   Byte 1:  bits 7..4 = Control Packet type (1..14; 0 and 15 reserved)
 *            bits 3..0 = type-specific flags
 *   Byte 2+: Remaining Length (variable length, max 4 octets)
 *            value = number of bytes after the fixed header
 *            (variable header + payload). Max = 268,435,455.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch127_marker[] = "libcgj-batch127";

/* MQTT 3.1.1 §2.2.3: maximum Remaining Length value. */
#define B127_REM_MAX  268435455u /* 0x0FFFFFFF */

/* ---- Remaining Length encode ------------------------------------------- */

/*
 * Encode rem with the MQTT variable-length scheme:
 *   do {
 *     encodedByte = X % 128;
 *     X = X / 128;
 *     if (X > 0) encodedByte |= 128;
 *     output encodedByte;
 *   } while (X > 0);
 * Always produces the minimal form (1..4 bytes).
 */
int
gj_mqtt_encode_remaining(unsigned char *out, size_t outcap, uint32_t rem,
                         size_t *n)
{
	uint32_t x;
	size_t i;

	if (out == NULL || n == NULL || rem > B127_REM_MAX) {
		return -1;
	}

	x = rem;
	i = 0u;
	for (;;) {
		unsigned char enc;

		if (i >= outcap) {
			return -1;
		}
		enc = (unsigned char)(x % 128u);
		x /= 128u;
		if (x > 0u) {
			enc = (unsigned char)(enc | 0x80u);
		}
		out[i] = enc;
		i++;
		if (x == 0u) {
			break;
		}
		/* Safety: never emit more than 4 octets. */
		if (i >= 4u) {
			return -1;
		}
	}

	*n = i;
	return 0;
}

/* ---- Remaining Length decode ------------------------------------------- */

/*
 * Decode Remaining Length (MQTT 3.1.1 §2.2.3 algorithm):
 *   multiplier = 1; value = 0;
 *   do {
 *     encodedByte = next byte;
 *     value += (encodedByte & 127) * multiplier;
 *     if (multiplier > 128*128*128) error;
 *     multiplier *= 128;
 *   } while (encodedByte & 128);
 */
int
gj_mqtt_decode_remaining(const unsigned char *in, size_t len, uint32_t *rem,
                         size_t *n)
{
	uint32_t uVal;
	uint32_t uMul;
	size_t i;

	if (in == NULL || rem == NULL || n == NULL || len == 0u) {
		return -1;
	}

	uVal = 0u;
	uMul = 1u;
	i = 0u;
	for (;;) {
		unsigned char enc;
		unsigned char dig;

		if (i >= len || i >= 4u) {
			return -1;
		}
		enc = in[i];
		i++;
		dig = (unsigned char)(enc & 0x7fu);
		/*
		 * Reject if dig * uMul would push past REM_MAX, or if a
		 * fifth continuation would be required (uMul already
		 * 128^3 and continuation bit set is checked below via
		 * the OASIS multiplier bound).
		 */
		if (dig > 0u && uMul > (B127_REM_MAX / (uint32_t)dig)) {
			return -1;
		}
		uVal += (uint32_t)dig * uMul;
		if (uVal > B127_REM_MAX) {
			return -1;
		}
		if ((enc & 0x80u) == 0u) {
			break;
		}
		/*
		 * Continuation set: next multiplier is uMul*128. Spec:
		 * if multiplier would exceed 128*128*128 after this
		 * byte, the encoding is malformed (would need 5 bytes).
		 */
		if (uMul > (128u * 128u * 128u)) {
			return -1;
		}
		uMul *= 128u;
	}

	*rem = uVal;
	*n = i;
	return 0;
}

/* ---- Fixed header parse ------------------------------------------------ */

/*
 * Parse the MQTT fixed header from in[0..len):
 *   type    = (in[0] >> 4) & 0x0f
 *   flags   = in[0] & 0x0f
 *   rem_len = Remaining Length field
 *   hdr_len = 1 + RL encoding size
 * Does not validate type-specific flag constraints (reserved bits),
 * only that the structural fixed header is present and well-formed.
 */
int
gj_mqtt_fixed_header(const unsigned char *in, size_t len, int *type,
                     int *flags, uint32_t *rem_len, size_t *hdr_len)
{
	uint32_t uRem;
	size_t nRl;

	/* Need type/flags byte plus at least one Remaining Length octet. */
	if (in == NULL || type == NULL || flags == NULL || rem_len == NULL ||
	    hdr_len == NULL || len < 2u) {
		return -1;
	}

	if (gj_mqtt_decode_remaining(in + 1, len - 1u, &uRem, &nRl) != 0) {
		return -1;
	}

	*type = (int)((in[0] >> 4) & 0x0fu);
	*flags = (int)(in[0] & 0x0fu);
	*rem_len = uRem;
	*hdr_len = 1u + nRl;
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_mqtt_encode_remaining(unsigned char *out, size_t outcap, uint32_t rem,
                               size_t *n)
    __attribute__((alias("gj_mqtt_encode_remaining")));
int __gj_mqtt_decode_remaining(const unsigned char *in, size_t len,
                               uint32_t *rem, size_t *n)
    __attribute__((alias("gj_mqtt_decode_remaining")));
int __gj_mqtt_fixed_header(const unsigned char *in, size_t len, int *type,
                           int *flags, uint32_t *rem_len, size_t *hdr_len)
    __attribute__((alias("gj_mqtt_fixed_header")));
