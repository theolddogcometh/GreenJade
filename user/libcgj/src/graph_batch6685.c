/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6685: rtnetlink RTN_ANYCAST route type stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtnl_rtn_anycast_6685(void);
 *     - Returns 4 (soft RTN_ANYCAST accept as local anycast type id).
 *       Pure-data catalog stub; not a live netlink/rtnl query.
 *   uint32_t __gj_rtnl_rtn_anycast_6685  (alias)
 *   __libcgj_batch6685_marker = "libcgj-batch6685"
 *
 * Exclusive continuum CREATE-ONLY (6681-6690: rtnetlink route type
 * stubs). Unique gj_rtnl_rtn_anycast_6685 surface only; no multi-def.
 * Distinct from broadcast (batch6684) and multicast (batch6686).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6685_marker[] = "libcgj-batch6685";

/* Soft RTN_ANYCAST (accept locally as anycast, send as anycast). */
#define B6685_RTN_ANYCAST  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6685_anycast(void)
{
	return B6685_RTN_ANYCAST;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtnl_rtn_anycast_6685 - soft RTN_ANYCAST route type catalog id.
 *
 * Always returns 4. Pure-data stub for product route-type catalogs;
 * does not open netlink sockets or parse nlmsg. No parent wires.
 */
uint32_t
gj_rtnl_rtn_anycast_6685(void)
{
	(void)NULL;
	return b6685_anycast();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtnl_rtn_anycast_6685(void)
    __attribute__((alias("gj_rtnl_rtn_anycast_6685")));
