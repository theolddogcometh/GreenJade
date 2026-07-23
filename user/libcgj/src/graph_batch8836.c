/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8836: netlink NLMSG_ERROR type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nlmsg_error_id_8836(void);
 *     - Return soft NLMSG_ERROR control message type id (2).
 *   uint32_t __gj_nlmsg_error_id_8836  (alias)
 *   __libcgj_batch8836_marker = "libcgj-batch8836"
 *
 * Exclusive continuum CREATE-ONLY (8831-8840: netlink msg type id stubs —
 * nl_route_id, nl_addr_id, nl_link_id, nl_neigh_id, nlmsg_done_id,
 * nlmsg_error_id, nlmsg_ok_u, nl_pid_ok_u, netlink_ready_u,
 * batch_id_8840). Unique gj_nlmsg_error_id_8836 surface only; no multi-def.
 * Catalog id only; no live netlink socket I/O. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8836_marker[] = "libcgj-batch8836";

/* Soft NLMSG_ERROR control type (ACK / error response). */
#define B8836_NLMSG_ERROR  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8836_nlmsg_error_id(void)
{
	return B8836_NLMSG_ERROR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nlmsg_error_id_8836 - soft NLMSG_ERROR control type id.
 *
 * Always returns 2. Catalog id only; does not decode nlmsgerr payloads
 * or ACK netlink requests. No parent wires.
 */
uint32_t
gj_nlmsg_error_id_8836(void)
{
	(void)NULL;
	return b8836_nlmsg_error_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nlmsg_error_id_8836(void)
    __attribute__((alias("gj_nlmsg_error_id_8836")));
