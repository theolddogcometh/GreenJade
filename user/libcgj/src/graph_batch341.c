/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch341: MQTT Remaining Length encode/decode
 * (OASIS MQTT Version 3.1.1 / 5.0 §2.1.4 variable-length integer).
 *
 * Surface (unique symbols):
 *   int gj_mqtt_rl_encode(uint32_t v, uint8_t *out, size_t cap, size_t *n);
 *     — Encode v as MQTT Remaining Length into out[0..cap). Writes the
 *       minimal 1..4 byte form. On success *n = encoded byte count and
 *       returns 0. Returns -1 on NULL out/n, v > 268435455, or
 *       insufficient cap. *n is unchanged on failure.
 *   int gj_mqtt_rl_decode(const uint8_t *in, size_t nin, uint32_t *v,
 *                         size_t *nread);
 *     — Decode Remaining Length from in[0..nin). On success *v = value,
 *       *nread = bytes consumed (1..4), returns 0. Returns -1 on NULL
 *       args, truncated / >4-byte / out-of-range / non-minimal-continuation
 *       encodings past the OASIS multiplier bound. *v and *nread are
 *       unchanged on failure.
 *   __gj_mqtt_rl_encode / __gj_mqtt_rl_decode  (aliases)
 *   __libcgj_batch341_marker = "libcgj-batch341"
 *
 * Distinct from batch127 gj_mqtt_encode_remaining /
 * gj_mqtt_decode_remaining (different symbol names and argument order).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party MQTT source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch341_marker[] = "libcgj-batch341";

/* MQTT Remaining Length maximum: 268,435,455 (0x0FFFFFFF). */
#define B341_RL_MAX  268435455u

/* ---- encode ------------------------------------------------------------ */

/*
 * Encode with the MQTT variable-length integer scheme:
 *   do {
 *     encodedByte = X % 128;
 *     X = X / 128;
 *     if (X > 0) encodedByte |= 128;
 *     output encodedByte;
 *   } while (X > 0);
 * Always produces the minimal form (1..4 bytes).
 */
int
gj_mqtt_rl_encode(uint32_t u32V, uint8_t *pOut, size_t cbCap, size_t *pN)
{
	uint32_t u32X;
	size_t iByte;

	if (pOut == NULL || pN == NULL || u32V > B341_RL_MAX) {
		return -1;
	}

	u32X = u32V;
	iByte = 0u;
	for (;;) {
		uint8_t u8Enc;

		if (iByte >= cbCap) {
			return -1;
		}
		u8Enc = (uint8_t)(u32X % 128u);
		u32X /= 128u;
		if (u32X > 0u) {
			u8Enc = (uint8_t)(u8Enc | 0x80u);
		}
		pOut[iByte] = u8Enc;
		iByte++;
		if (u32X == 0u) {
			break;
		}
		/* Spec: never more than 4 octets. */
		if (iByte >= 4u) {
			return -1;
		}
	}

	*pN = iByte;
	return 0;
}

/* ---- decode ------------------------------------------------------------ */

/*
 * Decode Remaining Length (MQTT §2.1.4 / 3.1.1 §2.2.3 algorithm):
 *   multiplier = 1; value = 0;
 *   do {
 *     encodedByte = next byte;
 *     value += (encodedByte & 127) * multiplier;
 *     if (multiplier > 128*128*128) error;
 *     multiplier *= 128;
 *   } while (encodedByte & 128);
 */
int
gj_mqtt_rl_decode(const uint8_t *pIn, size_t cbIn, uint32_t *pV,
                  size_t *pNread)
{
	uint32_t u32Val;
	uint32_t u32Mul;
	size_t iByte;

	if (pIn == NULL || pV == NULL || pNread == NULL || cbIn == 0u) {
		return -1;
	}

	u32Val = 0u;
	u32Mul = 1u;
	iByte = 0u;
	for (;;) {
		uint8_t u8Enc;
		uint8_t u8Dig;

		if (iByte >= cbIn || iByte >= 4u) {
			return -1;
		}
		u8Enc = pIn[iByte];
		iByte++;
		u8Dig = (uint8_t)(u8Enc & 0x7fu);

		/* Reject digit * multiplier overflow past RL_MAX. */
		if (u8Dig > 0u && u32Mul > (B341_RL_MAX / (uint32_t)u8Dig)) {
			return -1;
		}
		u32Val += (uint32_t)u8Dig * u32Mul;
		if (u32Val > B341_RL_MAX) {
			return -1;
		}
		if ((u8Enc & 0x80u) == 0u) {
			break;
		}
		/*
		 * Continuation set: next multiplier is u32Mul*128. Spec:
		 * if multiplier would exceed 128*128*128, the encoding is
		 * malformed (would need a fifth byte).
		 */
		if (u32Mul > (128u * 128u * 128u)) {
			return -1;
		}
		u32Mul *= 128u;
	}

	*pV = u32Val;
	*pNread = iByte;
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_mqtt_rl_encode(uint32_t u32V, uint8_t *pOut, size_t cbCap, size_t *pN)
    __attribute__((alias("gj_mqtt_rl_encode")));
int __gj_mqtt_rl_decode(const uint8_t *pIn, size_t cbIn, uint32_t *pV,
                        size_t *pNread)
    __attribute__((alias("gj_mqtt_rl_decode")));
