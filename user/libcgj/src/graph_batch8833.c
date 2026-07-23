/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8833: netlink RTM link type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nl_link_id_8833(void);
 *     - Return soft RTM_NEWLINK netlink message type id (16).
 *   uint32_t __gj_nl_link_id_8833  (alias)
 *   __libcgj_batch8833_marker = "libcgj-batch8833"
 *
 * Exclusive continuum CREATE-ONLY (8831-8840: netlink msg type id stubs —
 * nl_route_id, nl_addr_id, nl_link_id, nl_neigh_id, nlmsg_done_id,
 * nlmsg_error_id, nlmsg_ok_u, nl_pid_ok_u, netlink_ready_u,
 * batch_id_8840). Unique gj_nl_link_id_8833 surface only; no multi-def.
 * Catalog id only; no live netlink socket I/O. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8833_marker[] = "libcgj-batch8833";

/* Soft RTM_NEWLINK (NETLINK_ROUTE family message type). */
#define B8833_RTM_NEWLINK  ((uint32_t)16u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8833_nl_link_id(void)
{
	return B8833_RTM_NEWLINK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nl_link_id_8833 - soft RTM_NEWLINK netlink msg type id.
 *
 * Always returns 16. Catalog id only; does not open a netlink socket or
 * issue RTM_GETLINK/RTM_NEWLINK. No parent wires.
 */
uint32_t
gj_nl_link_id_8833(void)
{
	(void)NULL;
	return b8833_nl_link_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nl_link_id_8833(void)
    __attribute__((alias("gj_nl_link_id_8833")));
