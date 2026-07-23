/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5762: jumbo-frame interface MTU stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_if_mtu_jumbo_5762(void);
 *     - Returns stub jumbo-frame MTU in octets (9000). Soft
 *       product-connect link MTU tag; not a live NIC query.
 *   uint32_t __gj_if_mtu_jumbo_5762  (alias)
 *   __libcgj_batch5762_marker = "libcgj-batch5762"
 *
 * Exclusive continuum CREATE-ONLY (5761-5770: network interface
 * MTU/link/speed stubs for product connect). Unique
 * gj_if_mtu_jumbo_5762 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5762_marker[] = "libcgj-batch5762";

/* Common jumbo-frame MTU (octets). */
#define B5762_IF_MTU_JUMBO  9000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5762_mtu_jumbo(void)
{
	return B5762_IF_MTU_JUMBO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_mtu_jumbo_5762 - report stub jumbo-frame interface MTU.
 *
 * Always returns 9000. Integer-only product-connect MTU stub; does not
 * query a real NIC. Does not call libc. No parent wires.
 */
uint32_t
gj_if_mtu_jumbo_5762(void)
{
	(void)NULL;
	return b5762_mtu_jumbo();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_if_mtu_jumbo_5762(void)
    __attribute__((alias("gj_if_mtu_jumbo_5762")));
