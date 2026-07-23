/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5767: interface link-speed soft-check stub.
 *
 * Surface (unique symbols):
 *   int gj_if_speed_ok_5767(uint32_t mbps);
 *     - Return 1 if mbps is a common Ethernet speed
 *       (10/100/1000/2500/5000/10000/25000/40000/100000). Else 0.
 *   int __gj_if_speed_ok_5767  (alias)
 *   __libcgj_batch5767_marker = "libcgj-batch5767"
 *
 * Exclusive continuum CREATE-ONLY (5761-5770: network interface
 * MTU/link/speed stubs for product connect). Unique
 * gj_if_speed_ok_5767 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5767_marker[] = "libcgj-batch5767";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if mbps matches a common Ethernet link speed. */
static int
b5767_speed_ok(uint32_t uMbps)
{
	switch (uMbps) {
	case 10u:
	case 100u:
	case 1000u:
	case 2500u:
	case 5000u:
	case 10000u:
	case 25000u:
	case 40000u:
	case 100000u:
		return 1;
	default:
		return 0;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_speed_ok_5767 - soft check that mbps is a common Ethernet speed.
 *
 * mbps: candidate negotiated link speed in megabits per second
 *
 * Returns 1 for 10/100/1000/2500/5000/10000/25000/40000/100000, else 0.
 * Product-connect speed gate; does not call libc. No parent wires.
 */
int
gj_if_speed_ok_5767(uint32_t uMbps)
{
	(void)NULL;
	return b5767_speed_ok(uMbps);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_if_speed_ok_5767(uint32_t uMbps)
    __attribute__((alias("gj_if_speed_ok_5767")));
