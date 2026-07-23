/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4464: fold Internet checksum sum to 16 bits.
 *
 * Surface (unique symbols):
 *   uint16_t gj_csum_fold_u(uint32_t sum);
 *     - Fold a multi-word partial ones-complement sum into 16 bits with
 *       end-around carry (RFC 1071 §4.1). Does NOT invert; pure fold.
 *   uint16_t __gj_csum_fold_u  (alias)
 *   __libcgj_batch4464_marker = "libcgj-batch4464"
 *
 * Exclusive continuum CREATE-ONLY (4461-4470). Distinct from
 * gj_inet_csum_fold_u (batch2172) and gj_ip_checksum_fold (batch124) —
 * unique gj_csum_fold_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4464_marker[] = "libcgj-batch4464";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * RFC 1071 end-around carry until only 16 bits of significance remain.
 * No ones-complement invert (see gj_csum_finish_u for ~fold).
 */
static uint16_t
b4464_fold(uint32_t u32Sum)
{
	while ((u32Sum >> 16) != 0u) {
		u32Sum = (u32Sum & 0xffffu) + (u32Sum >> 16);
	}
	return (uint16_t)u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_csum_fold_u - fold a 32-bit Internet checksum partial to 16 bits.
 *
 * sum: partial ones-complement accumulator (e.g. from gj_csum_add_u
 *      or gj_udp_pseudo_fold_u)
 *
 * Returns the end-around-folded 16-bit value (not inverted). Use
 * gj_csum_finish_u when the store-form ones-complement is required.
 */
uint16_t
gj_csum_fold_u(uint32_t sum)
{
	(void)NULL;
	return b4464_fold(sum);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_csum_fold_u(uint32_t sum)
    __attribute__((alias("gj_csum_fold_u")));
