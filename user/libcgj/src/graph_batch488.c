/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch488: freestanding 16-bit network-to-host swap.
 *
 * Surface (unique symbols):
 *   uint16_t gj_ntohs16(uint16_t x);
 *     — Same as htons under LE host assumption: always portable bswap.
 *       Network order is big-endian; ntoh/hton are symmetric on LE hosts.
 *   uint16_t __gj_ntohs16  (alias)
 *   __libcgj_batch488_marker = "libcgj-batch488"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Distinct from ntohs in byteorder.c
 * and from gj_htons16 in graph_batch486.c (unique symbol names).
 */

#include <stdint.h>

const char __libcgj_batch488_marker[] = "libcgj-batch488";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ntohs16 — 16-bit network-to-host (always swap under LE host assumption).
 *
 * Identical transform to htons on little-endian hosts: portable pure-C
 * bswap of the two octets. Self-contained (no cross-batch call).
 */
uint16_t
gj_ntohs16(uint16_t x)
{
	return (uint16_t)(((x & 0x00ffu) << 8) | ((x & 0xff00u) >> 8));
}

uint16_t __gj_ntohs16(uint16_t x)
    __attribute__((alias("gj_ntohs16")));
