/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5761: standard Ethernet interface MTU stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_if_mtu_std_5761(void);
 *     - Returns stub standard Ethernet MTU in octets (1500). Soft
 *       product-connect link MTU tag; not a live ioctl probe.
 *   uint32_t __gj_if_mtu_std_5761  (alias)
 *   __libcgj_batch5761_marker = "libcgj-batch5761"
 *
 * Exclusive continuum CREATE-ONLY (5761-5770: network interface
 * MTU/link/speed stubs for product connect). Unique
 * gj_if_mtu_std_5761 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5761_marker[] = "libcgj-batch5761";

/* Standard Ethernet MTU (octets). */
#define B5761_IF_MTU_STD  1500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5761_mtu_std(void)
{
	return B5761_IF_MTU_STD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_mtu_std_5761 - report stub standard Ethernet interface MTU.
 *
 * Always returns 1500. Integer-only product-connect MTU stub; does not
 * query a real NIC. Does not call libc. No parent wires.
 */
uint32_t
gj_if_mtu_std_5761(void)
{
	(void)NULL;
	return b5761_mtu_std();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_if_mtu_std_5761(void)
    __attribute__((alias("gj_if_mtu_std_5761")));
