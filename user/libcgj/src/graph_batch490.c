/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch490: freestanding IPv4 inclusive range test.
 *
 * Surface (unique symbols):
 *   int gj_ipv4_in_range(uint32_t ip_be, uint32_t start_be, uint32_t end_be);
 *     — All three arguments are IPv4 addresses in network (big-endian)
 *       wire order. Convert each to host order via portable bswap (LE host
 *       assumption), then inclusive-compare: return 1 iff
 *       start_host <= ip_host <= end_host, else 0.
 *   int __gj_ipv4_in_range  (alias)
 *   __libcgj_batch490_marker = "libcgj-batch490"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Self-contained bswap (no cross-batch
 * call to gj_ntohl32).
 */

#include <stdint.h>

const char __libcgj_batch490_marker[] = "libcgj-batch490";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b490_ntohl — portable 32-bit bswap (BE wire → LE host order).
 */
static uint32_t
b490_ntohl(uint32_t x)
{
	return ((x & 0x000000ffu) << 24) |
	       ((x & 0x0000ff00u) << 8) |
	       ((x & 0x00ff0000u) >> 8) |
	       ((x & 0xff000000u) >> 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_in_range — inclusive IPv4 range membership after host-order load.
 *
 * Edge cases:
 *   start_be > end_be in host order → always 0 (empty range)
 *   single-address range (start == end) → 1 iff ip matches
 *   Returns 1 when in range, 0 otherwise.
 */
int
gj_ipv4_in_range(uint32_t ip_be, uint32_t start_be, uint32_t end_be)
{
	uint32_t ip_h;
	uint32_t start_h;
	uint32_t end_h;

	ip_h = b490_ntohl(ip_be);
	start_h = b490_ntohl(start_be);
	end_h = b490_ntohl(end_be);

	if (ip_h < start_h || ip_h > end_h) {
		return 0;
	}
	return 1;
}

int __gj_ipv4_in_range(uint32_t ip_be, uint32_t start_be, uint32_t end_be)
    __attribute__((alias("gj_ipv4_in_range")));
