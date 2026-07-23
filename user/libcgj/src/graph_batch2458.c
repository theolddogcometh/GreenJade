/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2458: host-order IPv4 network base address.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipv4_network_addr(uint32_t addr, unsigned prefix);
 *     - addr & netmask(prefix). prefix>32 -> 0. prefix==0 -> 0.
 *   uint32_t __gj_ipv4_network_addr  (alias)
 *   __libcgj_batch2458_marker = "libcgj-batch2458"
 *
 * Net exclusive wave (2451-2460). Distinct from gj_ipv4_network
 * (batch856). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2458_marker[] = "libcgj-batch2458";

/* ---- freestanding helpers ---------------------------------------------- */

/* Host-order netmask for a valid prefix in 1..32. */
static uint32_t
b2458_netmask(unsigned uPrefix)
{
	return 0xffffffffu << (32u - uPrefix);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_network_addr - network base for host-order addr/prefix.
 *
 * addr:   host-order IPv4 address
 * prefix: CIDR length 0..32; >32 -> 0; ==0 -> 0.0.0.0
 *
 * Returns addr with host bits cleared. Does not call libc.
 */
uint32_t
gj_ipv4_network_addr(uint32_t u32Addr, unsigned uPrefix)
{
	(void)NULL;
	if (uPrefix > 32u) {
		return 0u;
	}
	if (uPrefix == 0u) {
		return 0u;
	}
	return u32Addr & b2458_netmask(uPrefix);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipv4_network_addr(uint32_t u32Addr, unsigned uPrefix)
    __attribute__((alias("gj_ipv4_network_addr")));
