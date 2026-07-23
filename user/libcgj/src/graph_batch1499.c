/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1499: classic IPv4 address class from host order.
 *
 * Surface (unique symbols):
 *   int gj_ipv4_addr_class(uint32_t addr_host);
 *     — Classify a host-order IPv4 address by historical class bits
 *       (high bits of the address as a 32-bit host integer):
 *         0 = Class A  (0xxxxxxx…)
 *         1 = Class B  (10xxxxxx…)
 *         2 = Class C  (110xxxxx…)
 *         3 = Class D  (1110xxxx… multicast)
 *         4 = Class E  (1111xxxx… experimental)
 *   int __gj_ipv4_addr_class  (alias)
 *   __libcgj_batch1499_marker = "libcgj-batch1499"
 *
 * Distinct from gj_ipv4_header_ok (batch124) and gj_ipv4_host_count
 * (batch858). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1499_marker[] = "libcgj-batch1499";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_addr_class — historical A/B/C/D/E class of a host-order IPv4.
 *
 * addr_host: IPv4 address in host integer order (e.g. after ntohl of the
 *            on-wire word). High bit is the class A/B selector.
 *
 * Returns 0..4 as documented above.
 */
int
gj_ipv4_addr_class(uint32_t addr_host)
{
	/* Class A: high bit clear. */
	if ((addr_host & 0x80000000u) == 0u) {
		return 0;
	}
	/* Class B: 10xxxxxx… */
	if ((addr_host & 0xc0000000u) == 0x80000000u) {
		return 1;
	}
	/* Class C: 110xxxxx… */
	if ((addr_host & 0xe0000000u) == 0xc0000000u) {
		return 2;
	}
	/* Class D: 1110xxxx… */
	if ((addr_host & 0xf0000000u) == 0xe0000000u) {
		return 3;
	}
	/* Class E: 1111xxxx… */
	return 4;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv4_addr_class(uint32_t addr_host)
    __attribute__((alias("gj_ipv4_addr_class")));
