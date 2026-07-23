/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2459: host-order IPv4 directed broadcast address.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipv4_broadcast_addr(uint32_t addr, unsigned prefix);
 *     - addr | ~netmask. prefix>32 -> 0xffffffff. prefix==0 -> all-ones.
 *   uint32_t __gj_ipv4_broadcast_addr  (alias)
 *   __libcgj_batch2459_marker = "libcgj-batch2459"
 *
 * Net exclusive wave (2451-2460). Distinct from gj_ipv4_broadcast
 * (batch857) and gj_ipv4_is_broadcast_u (batch2455). Unique surface only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2459_marker[] = "libcgj-batch2459";

/* ---- freestanding helpers ---------------------------------------------- */

/* Host-order netmask for a valid prefix in 1..32. */
static uint32_t
b2459_netmask(unsigned uPrefix)
{
	return 0xffffffffu << (32u - uPrefix);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_broadcast_addr - directed broadcast for host-order addr/prefix.
 *
 * addr:   host-order IPv4 address
 * prefix: CIDR length
 *   prefix 0   -> 0xffffffff (broadcast of 0.0.0.0/0)
 *   prefix 32  -> addr (no host bits)
 *   prefix >32 -> 0xffffffff (invalid fallback)
 *
 * Does not call libc.
 */
uint32_t
gj_ipv4_broadcast_addr(uint32_t u32Addr, unsigned uPrefix)
{
	uint32_t u32Mask;

	(void)NULL;
	if (uPrefix > 32u) {
		return 0xffffffffu;
	}
	if (uPrefix == 0u) {
		return 0xffffffffu;
	}
	u32Mask = b2459_netmask(uPrefix);
	return u32Addr | ~u32Mask;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipv4_broadcast_addr(uint32_t u32Addr, unsigned uPrefix)
    __attribute__((alias("gj_ipv4_broadcast_addr")));
