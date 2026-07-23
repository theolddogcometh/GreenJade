/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5765: interface carrier-detect stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_if_carrier_ok_5765(void);
 *     - Returns 1 (stub physical carrier present). Soft product-connect
 *       carrier lamp; not a live netlink/ethtool carrier query.
 *   uint32_t __gj_if_carrier_ok_5765  (alias)
 *   __libcgj_batch5765_marker = "libcgj-batch5765"
 *
 * Exclusive continuum CREATE-ONLY (5761-5770: network interface
 * MTU/link/speed stubs for product connect). Unique
 * gj_if_carrier_ok_5765 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5765_marker[] = "libcgj-batch5765";

/* Stub carrier-present lamp (1 = carrier ok). */
#define B5765_IF_CARRIER_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5765_carrier_ok(void)
{
	return B5765_IF_CARRIER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_carrier_ok_5765 - report stub interface carrier presence.
 *
 * Always returns 1 (carrier ok). Integer-only product-connect carrier
 * stub; does not probe a real PHY. Does not call libc. No parent wires.
 */
uint32_t
gj_if_carrier_ok_5765(void)
{
	(void)NULL;
	return b5765_carrier_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_if_carrier_ok_5765(void)
    __attribute__((alias("gj_if_carrier_ok_5765")));
