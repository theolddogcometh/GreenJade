/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4462: UDP/TCP pseudo-header field fold (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_udp_pseudo_fold_u(uint32_t src, uint32_t dst,
 *                                 uint32_t len, uint32_t proto);
 *     - Fold IPv4 pseudo-header u32 fields into a 32-bit partial sum
 *       start: high/low 16-bit halves of src and dst plus low 16 bits
 *       of len and proto (RFC 768 / 793 style contribution).
 *   uint32_t __gj_udp_pseudo_fold_u  (alias)
 *   __libcgj_batch4462_marker = "libcgj-batch4462"
 *
 * Exclusive continuum CREATE-ONLY (4461-4470). Distinct from
 * gj_udp_csum_pseudo_v4_len (batch2174) and gj_udp_checksum_pseudo_len
 * (batch1117) — unique fold-of-fields surface; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4462_marker[] = "libcgj-batch4462";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Sum 16-bit halves of src/dst addresses plus len and proto words into
 * a 32-bit partial ones-complement accumulator (not finished).
 */
static uint32_t
b4462_pseudo_fold(uint32_t u32Src, uint32_t u32Dst, uint32_t u32Len,
    uint32_t u32Proto)
{
	uint32_t u32Sum;

	u32Sum = (u32Src >> 16) + (u32Src & 0xffffu);
	u32Sum += (u32Dst >> 16) + (u32Dst & 0xffffu);
	u32Sum += (u32Len & 0xffffu);
	u32Sum += (u32Proto & 0xffffu);
	return u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udp_pseudo_fold_u - start a UDP/TCP IPv4 pseudo-header partial sum.
 *
 * src:   IPv4 source address as host uint32_t (network-word halves)
 * dst:   IPv4 destination address as host uint32_t
 * len:   transport length field (low 16 bits used)
 * proto: IP protocol number (low 16 bits used; e.g. 17 for UDP)
 *
 * Returns 32-bit partial sum for further csum_add / fold / finish.
 * Self-contained; no parent wires.
 */
uint32_t
gj_udp_pseudo_fold_u(uint32_t src, uint32_t dst, uint32_t len, uint32_t proto)
{
	(void)NULL;
	return b4462_pseudo_fold(src, dst, len, proto);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udp_pseudo_fold_u(uint32_t src, uint32_t dst, uint32_t len,
    uint32_t proto)
    __attribute__((alias("gj_udp_pseudo_fold_u")));
