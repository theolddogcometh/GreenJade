/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8832: netlink RTM address type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nl_addr_id_8832(void);
 *     - Return soft RTM_NEWADDR netlink message type id (20).
 *   uint32_t __gj_nl_addr_id_8832  (alias)
 *   __libcgj_batch8832_marker = "libcgj-batch8832"
 *
 * Exclusive continuum CREATE-ONLY (8831-8840: netlink msg type id stubs —
 * nl_route_id, nl_addr_id, nl_link_id, nl_neigh_id, nlmsg_done_id,
 * nlmsg_error_id, nlmsg_ok_u, nl_pid_ok_u, netlink_ready_u,
 * batch_id_8840). Unique gj_nl_addr_id_8832 surface only; no multi-def.
 * Catalog id only; no live netlink socket I/O. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8832_marker[] = "libcgj-batch8832";

/* Soft RTM_NEWADDR (NETLINK_ROUTE family message type). */
#define B8832_RTM_NEWADDR  ((uint32_t)20u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8832_nl_addr_id(void)
{
	return B8832_RTM_NEWADDR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nl_addr_id_8832 - soft RTM_NEWADDR netlink msg type id.
 *
 * Always returns 20. Catalog id only; does not open a netlink socket or
 * issue RTM_GETADDR/RTM_NEWADDR. No parent wires.
 */
uint32_t
gj_nl_addr_id_8832(void)
{
	(void)NULL;
	return b8832_nl_addr_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nl_addr_id_8832(void)
    __attribute__((alias("gj_nl_addr_id_8832")));
