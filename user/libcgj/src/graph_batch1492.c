/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1492: TCP pseudo-header length field packing.
 *
 * Surface (unique symbols):
 *   uint16_t gj_tcp_pseudo_len(size_t seglen);
 *     — Pack seglen into the 16-bit TCP Length field used by an IPv4
 *       or IPv6 transport pseudo-header (RFC 793 / RFC 2460). Returns
 *       (uint16_t)(seglen & 0xffffu) for contribution as a network-order
 *       word when building ones-complement checksums.
 *   uint16_t __gj_tcp_pseudo_len  (alias)
 *   __libcgj_batch1492_marker = "libcgj-batch1492"
 *
 * Distinct from gj_tcp_checksum_pseudo_len (batch1116) and
 * gj_tcp_v4_cksum (batch348) — unique packing helper; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1492_marker[] = "libcgj-batch1492";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tcp_pseudo_len — 16-bit length for TCP/UDP pseudo-header packing.
 *
 * seglen: full transport segment length (header + payload) in bytes.
 * Returns the low 16 bits for the pseudo-header TCP Length field.
 */
uint16_t
gj_tcp_pseudo_len(size_t seglen)
{
	return (uint16_t)(seglen & 0xffffu);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_tcp_pseudo_len(size_t seglen)
    __attribute__((alias("gj_tcp_pseudo_len")));
