/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6686: rtnetlink RTN_MULTICAST route type stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtnl_rtn_multicast_6686(void);
 *     - Returns 5 (soft RTN_MULTICAST may be a multicast type id).
 *       Pure-data catalog stub; not a live netlink/rtnl query.
 *   uint32_t __gj_rtnl_rtn_multicast_6686  (alias)
 *   __libcgj_batch6686_marker = "libcgj-batch6686"
 *
 * Exclusive continuum CREATE-ONLY (6681-6690: rtnetlink route type
 * stubs). Unique gj_rtnl_rtn_multicast_6686 surface only; no multi-def.
 * Distinct from anycast (batch6685) and blackhole (batch6687).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6686_marker[] = "libcgj-batch6686";

/* Soft RTN_MULTICAST (may be a multicast route). */
#define B6686_RTN_MULTICAST  5u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6686_multicast(void)
{
	return B6686_RTN_MULTICAST;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtnl_rtn_multicast_6686 - soft RTN_MULTICAST route type catalog id.
 *
 * Always returns 5. Pure-data stub for product route-type catalogs;
 * does not open netlink sockets or parse nlmsg. No parent wires.
 */
uint32_t
gj_rtnl_rtn_multicast_6686(void)
{
	(void)NULL;
	return b6686_multicast();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtnl_rtn_multicast_6686(void)
    __attribute__((alias("gj_rtnl_rtn_multicast_6686")));
