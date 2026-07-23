/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8840: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8840(void);
 *     - Returns the compile-time graph batch number for this TU (8840).
 *   uint32_t __gj_batch_id_8840  (alias)
 *   __libcgj_batch8840_marker = "libcgj-batch8840"
 *
 * Exclusive continuum CREATE-ONLY (8831-8840: netlink msg type id stubs —
 * nl_route_id, nl_addr_id, nl_link_id, nl_neigh_id, nlmsg_done_id,
 * nlmsg_error_id, nlmsg_ok_u, nl_pid_ok_u, netlink_ready_u,
 * batch_id_8840). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Soft pure-data catalog
 * only; no netlink syscalls. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8840_marker[] = "libcgj-batch8840";

#define B8840_BATCH_ID  8840u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8840_id(void)
{
	return B8840_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8840 - report this TU's graph batch number.
 *
 * Always returns 8840. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8840(void)
{
	(void)NULL;
	return b8840_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8840(void)
    __attribute__((alias("gj_batch_id_8840")));
