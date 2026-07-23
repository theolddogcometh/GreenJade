/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch852: IPv4 octets → host-order uint32.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipv4_from_octets(unsigned a, unsigned b, unsigned c, unsigned d);
 *     — Pack a.b.c.d into host-order u32: (a<<24)|(b<<16)|(c<<8)|d.
 *       Low 8 bits of each arg used (caller should validate with 851).
 *   uint32_t __gj_ipv4_from_octets  (alias)
 *   __libcgj_batch852_marker = "libcgj-batch852"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch852_marker[] = "libcgj-batch852";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_from_octets — pack four octets into host-order IPv4 uint32.
 *
 * Host order: numerical value 0xAABBCCDD for A.B.C.D (e.g. 192.168.0.1
 * → 0xc0a80001). Each argument is masked to 8 bits.
 */
uint32_t
gj_ipv4_from_octets(unsigned uA, unsigned uB, unsigned uC, unsigned uD)
{
	return ((uint32_t)(uA & 0xffu) << 24) |
	       ((uint32_t)(uB & 0xffu) << 16) |
	       ((uint32_t)(uC & 0xffu) << 8) |
	       ((uint32_t)(uD & 0xffu));
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipv4_from_octets(unsigned uA, unsigned uB, unsigned uC,
    unsigned uD)
    __attribute__((alias("gj_ipv4_from_octets")));
