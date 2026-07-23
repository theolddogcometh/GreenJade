/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2174: UDP IPv4 pseudo-header length field.
 *
 * Surface (unique symbols):
 *   uint16_t gj_udp_csum_pseudo_v4_len(size_t seglen);
 *     - Pack seglen into the 16-bit UDP length field used by an IPv4
 *       transport pseudo-header (RFC 768). Returns low 16 bits of seglen
 *       for contribution as a network-order word when building the
 *       ones-complement UDP checksum.
 *   uint16_t __gj_udp_csum_pseudo_v4_len  (alias)
 *   __libcgj_batch2174_marker = "libcgj-batch2174"
 *
 * Post-2170 net checksum exclusive wave (2171-2180). Distinct from
 * gj_udp_checksum_pseudo_len (batch1117) and gj_tcp_pseudo_len
 * (batch1492) - unique _v4_len surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2174_marker[] = "libcgj-batch2174";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b2174_len(size_t cbSeg)
{
	return (uint16_t)(cbSeg & 0xffffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udp_csum_pseudo_v4_len - 16-bit length for IPv4 UDP pseudo-header.
 *
 * seglen: full UDP datagram length (header + payload) in bytes.
 * Returns (uint16_t)(seglen & 0xffffu) for the pseudo-header length field.
 */
uint16_t
gj_udp_csum_pseudo_v4_len(size_t seglen)
{
	(void)NULL;
	return b2174_len(seglen);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_udp_csum_pseudo_v4_len(size_t seglen)
    __attribute__((alias("gj_udp_csum_pseudo_v4_len")));
