/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6688: rtnetlink RTN_UNREACHABLE route type stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtnl_rtn_unreachable_6688(void);
 *     - Returns 7 (soft RTN_UNREACHABLE destination unreachable type).
 *       Pure-data catalog stub; not a live netlink/rtnl query.
 *   uint32_t __gj_rtnl_rtn_unreachable_6688  (alias)
 *   __libcgj_batch6688_marker = "libcgj-batch6688"
 *
 * Exclusive continuum CREATE-ONLY (6681-6690: rtnetlink route type
 * stubs). Unique gj_rtnl_rtn_unreachable_6688 surface only; no multi-def.
 * Distinct from blackhole (batch6687) and type_ok (batch6689).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6688_marker[] = "libcgj-batch6688";

/* Soft RTN_UNREACHABLE (destination is unreachable). */
#define B6688_RTN_UNREACHABLE  7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6688_unreachable(void)
{
	return B6688_RTN_UNREACHABLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtnl_rtn_unreachable_6688 - soft RTN_UNREACHABLE route type id.
 *
 * Always returns 7. Pure-data stub for product route-type catalogs;
 * does not open netlink sockets or parse nlmsg. No parent wires.
 */
uint32_t
gj_rtnl_rtn_unreachable_6688(void)
{
	(void)NULL;
	return b6688_unreachable();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtnl_rtn_unreachable_6688(void)
    __attribute__((alias("gj_rtnl_rtn_unreachable_6688")));
