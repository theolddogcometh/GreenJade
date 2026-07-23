/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch856: IPv4 network address from host-order addr/prefix.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipv4_network(uint32_t addr, unsigned prefix);
 *     — addr & netmask(prefix). prefix>32 → 0. prefix==0 → 0.
 *   uint32_t __gj_ipv4_network  (alias)
 *   __libcgj_batch856_marker = "libcgj-batch856"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch856_marker[] = "libcgj-batch856";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_network — network base address for host-order addr/prefix.
 *
 * prefix > 32 → 0 (invalid). prefix 0 → 0.0.0.0.
 * Otherwise returns addr with host bits cleared.
 */
uint32_t
gj_ipv4_network(uint32_t u32Addr, unsigned uPrefix)
{
	uint32_t u32Mask;

	if (uPrefix > 32u) {
		return 0u;
	}
	if (uPrefix == 0u) {
		return 0u;
	}
	u32Mask = 0xffffffffu << (32u - uPrefix);
	return u32Addr & u32Mask;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipv4_network(uint32_t u32Addr, unsigned uPrefix)
    __attribute__((alias("gj_ipv4_network")));
