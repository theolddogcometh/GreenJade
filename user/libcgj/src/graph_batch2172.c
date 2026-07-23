/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2172: Internet checksum end-around fold (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_inet_csum_fold_u(uint32_t sum);
 *     - Fold a multi-word partial sum into 16 bits with end-around carry
 *       (RFC 1071 §4.1). Does NOT ones-complement; returns the folded
 *       16-bit sum so it can be reused as a partial or finished later.
 *   uint16_t __gj_inet_csum_fold_u  (alias)
 *   __libcgj_batch2172_marker = "libcgj-batch2172"
 *
 * Post-2170 net checksum exclusive wave (2171-2180). Distinct from
 * gj_ip_checksum_fold (batch124) which folds and ones-complements -
 * pure fold surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2172_marker[] = "libcgj-batch2172";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * RFC 1071 end-around carry: repeatedly add high 16 bits into low until
 * only 16 bits of significance remain. No ones-complement.
 */
static uint16_t
b2172_fold(uint32_t u32Sum)
{
	while ((u32Sum >> 16) != 0u) {
		u32Sum = (u32Sum & 0xffffu) + (u32Sum >> 16);
	}
	return (uint16_t)u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inet_csum_fold_u - fold a 32-bit Internet checksum partial to 16 bits.
 *
 * sum: partial ones-complement accumulator from gj_inet_csum_partial_u
 *      (or any multi-word sum of network-order 16-bit words)
 *
 * Returns the end-around-folded 16-bit value (not inverted). Use
 * gj_inet_csum_finish_u when the store-form ones-complement is required.
 */
uint16_t
gj_inet_csum_fold_u(uint32_t sum)
{
	(void)NULL;
	return b2172_fold(sum);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_inet_csum_fold_u(uint32_t sum)
    __attribute__((alias("gj_inet_csum_fold_u")));
