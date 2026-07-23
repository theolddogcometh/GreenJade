/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4937: extract IPv4 third octet (c in a.b.c.d).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ip4_c_u(uint32_t ip);
 *     - Return the third octet of host-order IPv4 word ip as 0..255
 *       ((ip >> 8) & 0xff).
 *   uint32_t __gj_ip4_c_u  (alias)
 *   __libcgj_batch4937_marker = "libcgj-batch4937"
 *
 * Exclusive continuum CREATE-ONLY socket-addr wave (4931-4940). Distinct
 * from gj_ip4_a_u / gj_ip4_b_u — unique gj_ip4_c_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4937_marker[] = "libcgj-batch4937";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4937_c(uint32_t u32Ip)
{
	return (u32Ip >> 8) & 0xffu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ip4_c_u - extract the third octet of a host-order IPv4 word.
 *
 * ip: host-order packed address (0xAABBCCDD = a.b.c.d)
 *
 * Returns c in 0..255. Self-contained; no parent wires.
 */
uint32_t
gj_ip4_c_u(uint32_t u32Ip)
{
	(void)NULL;
	return b4937_c(u32Ip);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ip4_c_u(uint32_t u32Ip)
    __attribute__((alias("gj_ip4_c_u")));
