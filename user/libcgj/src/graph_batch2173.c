/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2173: Internet checksum finish (fold + invert).
 *
 * Surface (unique symbols):
 *   uint16_t gj_inet_csum_finish_u(uint32_t sum);
 *     - Fold a multi-word partial sum with end-around carry, then
 *       ones-complement. Returns the value to store in a zeroed checksum
 *       field (RFC 1071). Zero partial sum yields 0xffff.
 *   uint16_t __gj_inet_csum_finish_u  (alias)
 *   __libcgj_batch2173_marker = "libcgj-batch2173"
 *
 * Post-2170 net checksum exclusive wave (2171-2180). Complements
 * gj_inet_csum_partial_u / gj_inet_csum_fold_u; distinct from
 * gj_ip_checksum_fold (batch124) by name - unique finish surface only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2173_marker[] = "libcgj-batch2173";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * End-around carry fold into 16 bits, then ones-complement. Empty
 * (zero) partial sum yields 0xffff - the Internet checksum of nothing.
 */
static uint16_t
b2173_finish(uint32_t u32Sum)
{
	while ((u32Sum >> 16) != 0u) {
		u32Sum = (u32Sum & 0xffffu) + (u32Sum >> 16);
	}
	return (uint16_t)~u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inet_csum_finish_u - finish a partial Internet checksum for storage.
 *
 * sum: partial ones-complement accumulator (e.g. from
 *      gj_inet_csum_partial_u)
 *
 * Returns the 16-bit store-form checksum (folded and inverted).
 */
uint16_t
gj_inet_csum_finish_u(uint32_t sum)
{
	(void)NULL;
	return b2173_finish(sum);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_inet_csum_finish_u(uint32_t sum)
    __attribute__((alias("gj_inet_csum_finish_u")));
