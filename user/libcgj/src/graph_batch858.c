/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch858: usable IPv4 host count for a CIDR prefix.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipv4_host_count(unsigned prefix);
 *     — 2^(32-prefix) - 2 for prefix in 0..31. prefix >= 32 → 0.
 *       prefix 0 → 0xfffffffe (2^32 - 2).
 *   uint32_t __gj_ipv4_host_count  (alias)
 *   __libcgj_batch858_marker = "libcgj-batch858"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch858_marker[] = "libcgj-batch858";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_host_count — classic usable-host count for a prefix length.
 *
 * prefix >= 32 → 0 (no host field / single address not counted here)
 * prefix 0     → 2^32 - 2 (avoid <<32)
 * otherwise    → (1 << (32-prefix)) - 2
 *
 * Note: /31 yields 0 under the classic -2 formula; /30 yields 2.
 */
uint32_t
gj_ipv4_host_count(unsigned uPrefix)
{
	if (uPrefix >= 32u) {
		return 0u;
	}
	if (uPrefix == 0u) {
		return 0xfffffffeu;
	}
	return (1u << (32u - uPrefix)) - 2u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipv4_host_count(unsigned uPrefix)
    __attribute__((alias("gj_ipv4_host_count")));
