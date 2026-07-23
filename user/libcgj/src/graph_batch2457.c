/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2457: host-bit count for an IPv4 CIDR prefix.
 *
 * Surface (unique symbols):
 *   unsigned gj_ipv4_host_bits(unsigned prefix);
 *     - Return 32 - prefix for prefix in 0..32. prefix > 32 -> 0.
 *   unsigned __gj_ipv4_host_bits  (alias)
 *   __libcgj_batch2457_marker = "libcgj-batch2457"
 *
 * Net exclusive wave (2451-2460). Distinct from gj_ipv4_host_count
 * (batch858, usable-host count with -2). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2457_marker[] = "libcgj-batch2457";

/* ---- freestanding helpers ---------------------------------------------- */

/* Host bits remaining after a network prefix (clamped). */
static unsigned
b2457_host_bits(unsigned uPrefix)
{
	if (uPrefix > 32u) {
		return 0u;
	}
	return 32u - uPrefix;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_host_bits - number of host bits for a CIDR prefix length.
 *
 * prefix: network prefix length in bits (0..32). Values above 32
 * return 0 (invalid). Examples: /24 -> 8, /32 -> 0, /0 -> 32.
 * Does not call libc.
 */
unsigned
gj_ipv4_host_bits(unsigned uPrefix)
{
	(void)NULL;
	return b2457_host_bits(uPrefix);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_ipv4_host_bits(unsigned uPrefix)
    __attribute__((alias("gj_ipv4_host_bits")));
