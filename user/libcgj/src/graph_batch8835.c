/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8835: netlink NLMSG_DONE type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nlmsg_done_id_8835(void);
 *     - Return soft NLMSG_DONE control message type id (3).
 *   uint32_t __gj_nlmsg_done_id_8835  (alias)
 *   __libcgj_batch8835_marker = "libcgj-batch8835"
 *
 * Exclusive continuum CREATE-ONLY (8831-8840: netlink msg type id stubs —
 * nl_route_id, nl_addr_id, nl_link_id, nl_neigh_id, nlmsg_done_id,
 * nlmsg_error_id, nlmsg_ok_u, nl_pid_ok_u, netlink_ready_u,
 * batch_id_8840). Unique gj_nlmsg_done_id_8835 surface only; no multi-def.
 * Catalog id only; no live netlink socket I/O. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8835_marker[] = "libcgj-batch8835";

/* Soft NLMSG_DONE control type (end of multipart dump). */
#define B8835_NLMSG_DONE  ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8835_nlmsg_done_id(void)
{
	return B8835_NLMSG_DONE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nlmsg_done_id_8835 - soft NLMSG_DONE control type id.
 *
 * Always returns 3. Catalog id only; does not decode multipart netlink
 * dumps or consume NLMSG_DONE trailers. No parent wires.
 */
uint32_t
gj_nlmsg_done_id_8835(void)
{
	(void)NULL;
	return b8835_nlmsg_done_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nlmsg_done_id_8835(void)
    __attribute__((alias("gj_nlmsg_done_id_8835")));
