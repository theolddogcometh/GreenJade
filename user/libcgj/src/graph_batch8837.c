/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8837: netlink message type validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nlmsg_ok_u_8837(uint32_t nlmsg_type);
 *     - Return 1 if type is a known soft control or RTM family type
 *       in this continuum catalog, else 0.
 *   uint32_t __gj_nlmsg_ok_u_8837  (alias)
 *   __libcgj_batch8837_marker = "libcgj-batch8837"
 *
 * Exclusive continuum CREATE-ONLY (8831-8840: netlink msg type id stubs —
 * nl_route_id, nl_addr_id, nl_link_id, nl_neigh_id, nlmsg_done_id,
 * nlmsg_error_id, nlmsg_ok_u, nl_pid_ok_u, netlink_ready_u,
 * batch_id_8840). Unique gj_nlmsg_ok_u_8837 surface only; no multi-def.
 * Soft catalog check only; no live netlink socket I/O. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8837_marker[] = "libcgj-batch8837";

/*
 * Soft netlink type catalog for this continuum:
 *   NLMSG_ERROR (2) | NLMSG_DONE (3) |
 *   RTM_NEWLINK (16) | RTM_NEWADDR (20) | RTM_NEWROUTE (24) |
 *   RTM_NEWNEIGH (28)
 */
#define B8837_NLMSG_ERROR   ((uint32_t)2u)
#define B8837_NLMSG_DONE    ((uint32_t)3u)
#define B8837_RTM_NEWLINK   ((uint32_t)16u)
#define B8837_RTM_NEWADDR   ((uint32_t)20u)
#define B8837_RTM_NEWROUTE  ((uint32_t)24u)
#define B8837_RTM_NEWNEIGH  ((uint32_t)28u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8837_nlmsg_ok(uint32_t u32Type)
{
	if (u32Type == B8837_NLMSG_ERROR)
		return 1u;
	if (u32Type == B8837_NLMSG_DONE)
		return 1u;
	if (u32Type == B8837_RTM_NEWLINK)
		return 1u;
	if (u32Type == B8837_RTM_NEWADDR)
		return 1u;
	if (u32Type == B8837_RTM_NEWROUTE)
		return 1u;
	if (u32Type == B8837_RTM_NEWNEIGH)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nlmsg_ok_u_8837 - 1 if nlmsg_type is a known soft catalog type.
 *
 * nlmsg_type: soft netlink message type field
 *
 * Returns 1 when type is in {2, 3, 16, 20, 24, 28}, else 0. Soft catalog
 * check only; does not parse nlmsghdr. No parent wires.
 */
uint32_t
gj_nlmsg_ok_u_8837(uint32_t u32NlmsgType)
{
	(void)NULL;
	return b8837_nlmsg_ok(u32NlmsgType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nlmsg_ok_u_8837(uint32_t u32NlmsgType)
    __attribute__((alias("gj_nlmsg_ok_u_8837")));
