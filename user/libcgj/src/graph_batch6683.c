/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6683: rtnetlink RTN_LOCAL route type stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtnl_rtn_local_6683(void);
 *     - Returns 2 (soft RTN_LOCAL accept-locally route type id).
 *       Pure-data catalog stub; not a live netlink/rtnl query.
 *   uint32_t __gj_rtnl_rtn_local_6683  (alias)
 *   __libcgj_batch6683_marker = "libcgj-batch6683"
 *
 * Exclusive continuum CREATE-ONLY (6681-6690: rtnetlink route type
 * stubs). Unique gj_rtnl_rtn_local_6683 surface only; no multi-def.
 * Distinct from unicast (batch6682) and broadcast (batch6684).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6683_marker[] = "libcgj-batch6683";

/* Soft RTN_LOCAL (accept locally). */
#define B6683_RTN_LOCAL  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6683_local(void)
{
	return B6683_RTN_LOCAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtnl_rtn_local_6683 - soft RTN_LOCAL route type catalog id.
 *
 * Always returns 2. Pure-data stub for product route-type catalogs;
 * does not open netlink sockets or parse nlmsg. No parent wires.
 */
uint32_t
gj_rtnl_rtn_local_6683(void)
{
	(void)NULL;
	return b6683_local();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtnl_rtn_local_6683(void)
    __attribute__((alias("gj_rtnl_rtn_local_6683")));
