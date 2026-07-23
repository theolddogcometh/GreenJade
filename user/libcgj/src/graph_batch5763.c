/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5763: interface MTU range soft-check stub.
 *
 * Surface (unique symbols):
 *   int gj_if_mtu_ok_5763(uint32_t mtu);
 *     - Return 1 if mtu is a plausible link MTU in [68, 65535]
 *       (IPv4 min fragmentable through max IP total length). Else 0.
 *   int __gj_if_mtu_ok_5763  (alias)
 *   __libcgj_batch5763_marker = "libcgj-batch5763"
 *
 * Exclusive continuum CREATE-ONLY (5761-5770: network interface
 * MTU/link/speed stubs for product connect). Unique
 * gj_if_mtu_ok_5763 surface only; no multi-def. Distinct from
 * gj_net_mtu_ok_u (batch3128). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5763_marker[] = "libcgj-batch5763";

/* IPv4 min fragmentable MTU .. max IP total length. */
#define B5763_MTU_MIN  68u
#define B5763_MTU_MAX  65535u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if mtu is in [B5763_MTU_MIN, B5763_MTU_MAX]. */
static int
b5763_mtu_ok(uint32_t uMtu)
{
	if (uMtu < B5763_MTU_MIN) {
		return 0;
	}
	if (uMtu > B5763_MTU_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_mtu_ok_5763 - soft check that mtu is a plausible interface MTU.
 *
 * mtu: candidate maximum transmission unit in octets
 *
 * Returns 1 when mtu is in [68, 65535], else 0. Product-connect MTU
 * gate; does not call libc. No parent wires.
 */
int
gj_if_mtu_ok_5763(uint32_t uMtu)
{
	(void)NULL;
	return b5763_mtu_ok(uMtu);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_if_mtu_ok_5763(uint32_t uMtu)
    __attribute__((alias("gj_if_mtu_ok_5763")));
