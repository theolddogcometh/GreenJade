/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4465: finish Internet checksum (fold + ~).
 *
 * Surface (unique symbols):
 *   uint16_t gj_csum_finish_u(uint32_t sum);
 *     - Fold a multi-word partial sum with end-around carry, then
 *       ones-complement (~). Store-form checksum (RFC 1071). Zero
 *       partial sum yields 0xffff.
 *   uint16_t __gj_csum_finish_u  (alias)
 *   __libcgj_batch4465_marker = "libcgj-batch4465"
 *
 * Exclusive continuum CREATE-ONLY (4461-4470). Distinct from
 * gj_inet_csum_finish_u (batch2173) and gj_ip_checksum_fold_u
 * (batch3126) — unique gj_csum_finish_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4465_marker[] = "libcgj-batch4465";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * End-around carry fold into 16 bits, then ones-complement. Empty
 * (zero) partial sum yields 0xffff - the Internet checksum of nothing.
 */
static uint16_t
b4465_finish(uint32_t u32Sum)
{
	while ((u32Sum >> 16) != 0u) {
		u32Sum = (u32Sum & 0xffffu) + (u32Sum >> 16);
	}
	return (uint16_t)~u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_csum_finish_u - finish a partial Internet checksum for storage.
 *
 * sum: partial ones-complement accumulator (e.g. from gj_csum_add_u
 *      or gj_udp_pseudo_fold_u)
 *
 * Returns the 16-bit store-form checksum (folded and inverted).
 */
uint16_t
gj_csum_finish_u(uint32_t sum)
{
	(void)NULL;
	return b4465_finish(sum);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_csum_finish_u(uint32_t sum)
    __attribute__((alias("gj_csum_finish_u")));
