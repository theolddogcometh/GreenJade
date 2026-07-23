/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6684: rtnetlink RTN_BROADCAST route type stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtnl_rtn_broadcast_6684(void);
 *     - Returns 3 (soft RTN_BROADCAST accept/send broadcast type id).
 *       Pure-data catalog stub; not a live netlink/rtnl query.
 *   uint32_t __gj_rtnl_rtn_broadcast_6684  (alias)
 *   __libcgj_batch6684_marker = "libcgj-batch6684"
 *
 * Exclusive continuum CREATE-ONLY (6681-6690: rtnetlink route type
 * stubs). Unique gj_rtnl_rtn_broadcast_6684 surface only; no multi-def.
 * Distinct from anycast (batch6685) and multicast (batch6686).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6684_marker[] = "libcgj-batch6684";

/* Soft RTN_BROADCAST (accept locally as broadcast, send as broadcast). */
#define B6684_RTN_BROADCAST  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6684_broadcast(void)
{
	return B6684_RTN_BROADCAST;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtnl_rtn_broadcast_6684 - soft RTN_BROADCAST route type catalog id.
 *
 * Always returns 3. Pure-data stub for product route-type catalogs;
 * does not open netlink sockets or parse nlmsg. No parent wires.
 */
uint32_t
gj_rtnl_rtn_broadcast_6684(void)
{
	(void)NULL;
	return b6684_broadcast();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtnl_rtn_broadcast_6684(void)
    __attribute__((alias("gj_rtnl_rtn_broadcast_6684")));
