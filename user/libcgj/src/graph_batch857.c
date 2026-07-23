/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch857: IPv4 broadcast address from host-order addr/prefix.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipv4_broadcast(uint32_t addr, unsigned prefix);
 *     — addr | ~netmask. prefix>32 → 0xffffffff. prefix==0 → all-ones.
 *   uint32_t __gj_ipv4_broadcast  (alias)
 *   __libcgj_batch857_marker = "libcgj-batch857"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch857_marker[] = "libcgj-batch857";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_broadcast — directed broadcast for host-order addr/prefix.
 *
 * prefix 0  → 0xffffffff (broadcast of 0.0.0.0/0)
 * prefix 32 → addr (no host bits)
 * prefix >32 → 0xffffffff (treat as full host mask fallback)
 */
uint32_t
gj_ipv4_broadcast(uint32_t u32Addr, unsigned uPrefix)
{
	uint32_t u32Mask;

	if (uPrefix > 32u) {
		return 0xffffffffu;
	}
	if (uPrefix == 0u) {
		return 0xffffffffu;
	}
	u32Mask = 0xffffffffu << (32u - uPrefix);
	return u32Addr | ~u32Mask;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipv4_broadcast(uint32_t u32Addr, unsigned uPrefix)
    __attribute__((alias("gj_ipv4_broadcast")));
