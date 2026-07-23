/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5069: Internet checksum end-around fold.
 *
 * Surface (unique symbols):
 *   uint16_t gj_net_checksum_fold_u(uint32_t sum);
 *     - Fold carries of a partial 32-bit ones-complement sum into
 *       16 bits (RFC 1071 §4.1 end-around carry), then ones-complement.
 *       Zero partial sum yields 0xffff (checksum of empty buffer).
 *   uint16_t __gj_net_checksum_fold_u  (alias)
 *   __libcgj_batch5069_marker = "libcgj-batch5069"
 *
 * Exclusive continuum CREATE-ONLY (5061-5070). Distinct from
 * gj_ip_checksum_fold (batch124) / gj_ip_checksum_fold_u (batch3126)
 * by the gj_net_checksum_fold_u name — unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5069_marker[] = "libcgj-batch5069";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * End-around carry fold into 16 bits, then ones-complement.
 * Zero partial sum yields 0xffff.
 */
static uint16_t
b5069_fold(uint32_t u32Sum)
{
	while ((u32Sum >> 16) != 0u) {
		u32Sum = (u32Sum & 0xffffu) + (u32Sum >> 16);
	}
	return (uint16_t)~u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_checksum_fold_u - fold a 32-bit partial sum and ones-complement.
 *
 * sum: partial ones-complement accumulator of network-order 16-bit words
 *
 * Returns the 16-bit store-form Internet checksum (folded and inverted).
 * No parent wires.
 */
uint16_t
gj_net_checksum_fold_u(uint32_t sum)
{
	(void)NULL;
	return b5069_fold(sum);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_net_checksum_fold_u(uint32_t sum)
    __attribute__((alias("gj_net_checksum_fold_u")));
