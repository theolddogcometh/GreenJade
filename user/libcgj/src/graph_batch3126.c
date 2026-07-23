/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3126: gj_ip_checksum_fold_u - fold + invert.
 *
 * Surface (unique symbols):
 *   uint16_t gj_ip_checksum_fold_u(uint32_t sum);
 *     - Fold carries of a partial 32-bit ones-complement sum into
 *       16 bits (RFC 1071 §4.1 end-around carry), then ones-complement.
 *       Zero partial sum yields 0xffff (checksum of empty buffer).
 *   uint16_t __gj_ip_checksum_fold_u  (alias)
 *   __libcgj_batch3126_marker = "libcgj-batch3126"
 *
 * Exclusive checksum/net CREATE-ONLY wave (3121-3130). Distinct from
 * gj_ip_checksum_fold (batch124) by the _u name — unique surface only;
 * no multi-def. Self-contained; no parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3126_marker[] = "libcgj-batch3126";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * End-around carry fold into 16 bits, then ones-complement.
 * Zero partial sum yields 0xffff.
 */
static uint16_t
b3126_fold(uint32_t u32Sum)
{
	while ((u32Sum >> 16) != 0u) {
		u32Sum = (u32Sum & 0xffffu) + (u32Sum >> 16);
	}
	return (uint16_t)~u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ip_checksum_fold_u - fold a 32-bit partial sum and ones-complement.
 *
 * sum: partial ones-complement accumulator of network-order 16-bit words
 *
 * Returns the 16-bit store-form Internet checksum (folded and inverted).
 * No parent wires.
 */
uint16_t
gj_ip_checksum_fold_u(uint32_t sum)
{
	(void)NULL;
	return b3126_fold(sum);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_ip_checksum_fold_u(uint32_t sum)
    __attribute__((alias("gj_ip_checksum_fold_u")));
