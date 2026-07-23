/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6687: rtnetlink RTN_BLACKHOLE route type stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtnl_rtn_blackhole_6687(void);
 *     - Returns 6 (soft RTN_BLACKHOLE drop silently type id).
 *       Pure-data catalog stub; not a live netlink/rtnl query.
 *   uint32_t __gj_rtnl_rtn_blackhole_6687  (alias)
 *   __libcgj_batch6687_marker = "libcgj-batch6687"
 *
 * Exclusive continuum CREATE-ONLY (6681-6690: rtnetlink route type
 * stubs). Unique gj_rtnl_rtn_blackhole_6687 surface only; no multi-def.
 * Distinct from multicast (batch6686) and unreachable (batch6688).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6687_marker[] = "libcgj-batch6687";

/* Soft RTN_BLACKHOLE (drop packets without notification). */
#define B6687_RTN_BLACKHOLE  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6687_blackhole(void)
{
	return B6687_RTN_BLACKHOLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtnl_rtn_blackhole_6687 - soft RTN_BLACKHOLE route type catalog id.
 *
 * Always returns 6. Pure-data stub for product route-type catalogs;
 * does not open netlink sockets or parse nlmsg. No parent wires.
 */
uint32_t
gj_rtnl_rtn_blackhole_6687(void)
{
	(void)NULL;
	return b6687_blackhole();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtnl_rtn_blackhole_6687(void)
    __attribute__((alias("gj_rtnl_rtn_blackhole_6687")));
