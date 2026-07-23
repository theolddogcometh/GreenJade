/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch854: CIDR prefix → host-order IPv4 netmask.
 *
 * Surface (unique symbols):
 *   uint32_t gj_netmask_from_prefix(unsigned prefix);
 *     — prefix 0 → 0; 1..32 → left-aligned ones; prefix > 32 → 0.
 *   uint32_t __gj_netmask_from_prefix  (alias)
 *   __libcgj_batch854_marker = "libcgj-batch854"
 *
 * Distinct from gj_netmask_to_prefix (batch336, BE) — unique name.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch854_marker[] = "libcgj-batch854";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_netmask_from_prefix — host-order netmask for CIDR prefix length.
 *
 * prefix 0  → 0x00000000
 * prefix 24 → 0xffffff00
 * prefix 32 → 0xffffffff
 * prefix >32 → 0 (invalid)
 *
 * Avoids undefined 32-bit left-shift-by-32 for prefix 0.
 */
uint32_t
gj_netmask_from_prefix(unsigned uPrefix)
{
	if (uPrefix > 32u) {
		return 0u;
	}
	if (uPrefix == 0u) {
		return 0u;
	}
	return 0xffffffffu << (32u - uPrefix);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_netmask_from_prefix(unsigned uPrefix)
    __attribute__((alias("gj_netmask_from_prefix")));
