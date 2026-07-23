/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2180: milestone 2180 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2180(void);
 *     - Returns the compile-time graph batch number for this TU (2180).
 *   uint32_t gj_graph_milestone_2180(void);
 *     - Returns the current graph milestone revision (2180).
 *   uint32_t __gj_batch_id_2180  (alias)
 *   uint32_t __gj_graph_milestone_2180  (alias)
 *   __libcgj_batch2180_marker = "libcgj-batch2180"
 *
 * Milestone for the post-2170 net checksum exclusive wave (batches
 * 2171-2179: inet_csum_partial_u, inet_csum_fold_u, inet_csum_finish_u,
 * udp_csum_pseudo_v4_len, tcp_csum_pseudo_v4_len, icmp_type_echo_p,
 * icmp_type_echoreply_p, ip_proto_tcp_p, ip_proto_udp_p). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2180_marker[] = "libcgj-batch2180";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2180_id(void)
{
	return 2180u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2180 - report this TU's graph batch number.
 *
 * Always returns 2180.
 */
uint32_t
gj_batch_id_2180(void)
{
	(void)NULL;
	return b2180_id();
}

/*
 * gj_graph_milestone_2180 - report this TU's graph milestone revision.
 *
 * Always returns 2180 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2180(void)
{
	return b2180_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2180(void)
    __attribute__((alias("gj_batch_id_2180")));

uint32_t __gj_graph_milestone_2180(void)
    __attribute__((alias("gj_graph_milestone_2180")));
