/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5766: interface link speed (Mbps) stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_if_speed_mbps_5766(void);
 *     - Returns stub negotiated link speed in megabits per second
 *       (1000 = 1 Gbps). Soft product-connect speed tag; not ethtool.
 *   uint32_t __gj_if_speed_mbps_5766  (alias)
 *   __libcgj_batch5766_marker = "libcgj-batch5766"
 *
 * Exclusive continuum CREATE-ONLY (5761-5770: network interface
 * MTU/link/speed stubs for product connect). Unique
 * gj_if_speed_mbps_5766 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5766_marker[] = "libcgj-batch5766";

/* Stub negotiated link speed: 1000 Mbps (1 Gbps). */
#define B5766_IF_SPEED_MBPS  1000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5766_speed_mbps(void)
{
	return B5766_IF_SPEED_MBPS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_speed_mbps_5766 - report stub interface link speed (Mbps).
 *
 * Always returns 1000. Integer-only product-connect speed stub; does
 * not query a real NIC. Does not call libc. No parent wires.
 */
uint32_t
gj_if_speed_mbps_5766(void)
{
	(void)NULL;
	return b5766_speed_mbps();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_if_speed_mbps_5766(void)
    __attribute__((alias("gj_if_speed_mbps_5766")));
