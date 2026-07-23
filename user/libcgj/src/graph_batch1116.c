/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1116: TCP pseudo-header length field.
 *
 * Surface (unique symbols):
 *   uint16_t gj_tcp_checksum_pseudo_len(size_t seglen);
 *     — Return the 16-bit TCP length field value for an IPv4/IPv6
 *       transport pseudo-header (low 16 bits of seglen). Used when
 *       building RFC 793 / RFC 2460 checksum contributions.
 *   uint16_t __gj_tcp_checksum_pseudo_len  (alias)
 *   __libcgj_batch1116_marker = "libcgj-batch1116"
 *
 * Distinct from gj_tcp_v4_cksum (batch348) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1116_marker[] = "libcgj-batch1116";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tcp_checksum_pseudo_len — 16-bit length for TCP pseudo-header.
 *
 * seglen: full TCP segment length (header + payload) in bytes.
 * Returns (uint16_t)(seglen & 0xffffu).
 */
uint16_t
gj_tcp_checksum_pseudo_len(size_t cbSeg)
{
	return (uint16_t)(cbSeg & 0xffffu);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_tcp_checksum_pseudo_len(size_t cbSeg)
    __attribute__((alias("gj_tcp_checksum_pseudo_len")));
