/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2832: pack four octets into host-order IPv4 (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipv4_pack_u(unsigned a, unsigned b, unsigned c, unsigned d);
 *     - Pack a.b.c.d into host-order u32: (a<<24)|(b<<16)|(c<<8)|d.
 *       Each argument is masked to 8 bits.
 *   uint32_t __gj_ipv4_pack_u  (alias)
 *   __libcgj_batch2832_marker = "libcgj-batch2832"
 *
 * IPv4/endian exclusive wave (2831-2840). Distinct from
 * gj_ipv4_from_octets (batch852) — unique pack_u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2832_marker[] = "libcgj-batch2832";

/* ---- freestanding helpers ---------------------------------------------- */

/* Pack four octets into host-order IPv4 (low 8 bits of each arg). */
static uint32_t
b2832_pack(unsigned uA, unsigned uB, unsigned uC, unsigned uD)
{
	return ((uint32_t)(uA & 0xffu) << 24) |
	       ((uint32_t)(uB & 0xffu) << 16) |
	       ((uint32_t)(uC & 0xffu) << 8) |
	       ((uint32_t)(uD & 0xffu));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_pack_u - pack four octets into a host-order IPv4 uint32.
 *
 * a,b,c,d: address octets (only low 8 bits used; pair with
 *          gj_ipv4_ok_octet_u when full validation is required)
 *
 * Host-order layout: numerical 0xAABBCCDD for A.B.C.D
 * (e.g. 192.168.0.1 -> 0xc0a80001). Does not call libc.
 */
uint32_t
gj_ipv4_pack_u(unsigned uA, unsigned uB, unsigned uC, unsigned uD)
{
	(void)NULL;
	return b2832_pack(uA, uB, uC, uD);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipv4_pack_u(unsigned uA, unsigned uB, unsigned uC, unsigned uD)
    __attribute__((alias("gj_ipv4_pack_u")));
