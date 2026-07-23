/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6689: rtnetlink route type catalog range check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtnl_rtn_type_ok_6689(uint32_t rtm_type);
 *     - Returns 1 when rtm_type is in soft RTN catalog range
 *       [0, 12) (__RTN_MAX exclusive), else 0. Pure-data range stub;
 *       not a live fib/rtnl validation.
 *   uint32_t __gj_rtnl_rtn_type_ok_6689  (alias)
 *   __libcgj_batch6689_marker = "libcgj-batch6689"
 *
 * Exclusive continuum CREATE-ONLY (6681-6690: rtnetlink route type
 * stubs —
 * rtn_unspec_6681, rtn_unicast_6682, rtn_local_6683,
 * rtn_broadcast_6684, rtn_anycast_6685, rtn_multicast_6686,
 * rtn_blackhole_6687, rtn_unreachable_6688, rtn_type_ok_6689,
 * continuum + batch_id_6690). Unique surface only; no multi-def.
 * Soft bound covers RTN_UNSPEC..RTN_XRESOLVE. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6689_marker[] = "libcgj-batch6689";

/* Soft __RTN_MAX exclusive ceiling (RTN_UNSPEC..RTN_XRESOLVE). */
#define B6689_RTN_MAX  12u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6689_type_ok(uint32_t uType)
{
	return (uType < B6689_RTN_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtnl_rtn_type_ok_6689 - soft RTN route type catalog membership.
 *
 * rtm_type: candidate rtmsg rtm_type / fib route type id.
 * Returns 1 if rtm_type is in [0, 12), else 0. Does not query the
 * kernel. No parent wires.
 */
uint32_t
gj_rtnl_rtn_type_ok_6689(uint32_t rtm_type)
{
	(void)NULL;
	return b6689_type_ok(rtm_type);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtnl_rtn_type_ok_6689(uint32_t rtm_type)
    __attribute__((alias("gj_rtnl_rtn_type_ok_6689")));
