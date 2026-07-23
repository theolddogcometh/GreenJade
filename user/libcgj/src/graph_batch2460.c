/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2460: milestone 2460 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2460(void);
 *     - Returns the compile-time graph batch number for this TU (2460).
 *   uint32_t gj_graph_milestone_2460(void);
 *     - Returns the current graph milestone revision (2460).
 *   uint32_t __gj_batch_id_2460  (alias)
 *   uint32_t __gj_graph_milestone_2460  (alias)
 *   __libcgj_batch2460_marker = "libcgj-batch2460"
 *
 * Milestone for the exclusive net helpers wave (batches 2451-2459:
 * loopback_u, private_u, linklocal_u, multicast_u, broadcast_u,
 * netmask_prefix, host_bits, network_addr, broadcast_addr). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2460_marker[] = "libcgj-batch2460";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2460_id(void)
{
	return 2460u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2460 - report this TU's graph batch number.
 *
 * Always returns 2460.
 */
uint32_t
gj_batch_id_2460(void)
{
	(void)NULL;
	return b2460_id();
}

/*
 * gj_graph_milestone_2460 - report this TU's graph milestone revision.
 *
 * Always returns 2460 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2460(void)
{
	return b2460_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2460(void)
    __attribute__((alias("gj_batch_id_2460")));

uint32_t __gj_graph_milestone_2460(void)
    __attribute__((alias("gj_graph_milestone_2460")));
