/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3128: gj_net_mtu_ok_u - IPv4 link MTU range.
 *
 * Surface (unique symbols):
 *   int gj_net_mtu_ok_u(uint32_t mtu);
 *     - Return 1 if mtu is a plausible IPv4 link MTU in the closed
 *       range [68, 65535] (RFC 791 minimum fragmentable datagram size
 *       through max IP total length). Else 0.
 *   int __gj_net_mtu_ok_u  (alias)
 *   __libcgj_batch3128_marker = "libcgj-batch3128"
 *
 * Exclusive checksum/net CREATE-ONLY wave (3121-3130). Unique
 * gj_net_mtu_ok_u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3128_marker[] = "libcgj-batch3128";

/* IPv4 min fragmentable MTU (RFC 791) .. max IP total length. */
#define B3128_MTU_MIN 68u
#define B3128_MTU_MAX 65535u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if mtu is in [B3128_MTU_MIN, B3128_MTU_MAX]. */
static int
b3128_mtu_ok(uint32_t uMtu)
{
	if (uMtu < B3128_MTU_MIN) {
		return 0;
	}
	if (uMtu > B3128_MTU_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_mtu_ok_u - soft check that mtu is a plausible IPv4 link MTU.
 *
 * mtu: candidate maximum transmission unit in octets
 *
 * Returns 1 when mtu is in [68, 65535], else 0. No parent wires.
 */
int
gj_net_mtu_ok_u(uint32_t uMtu)
{
	(void)NULL;
	return b3128_mtu_ok(uMtu);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_net_mtu_ok_u(uint32_t uMtu)
    __attribute__((alias("gj_net_mtu_ok_u")));
