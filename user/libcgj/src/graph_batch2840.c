/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2840: milestone 2840 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2840(void);
 *     - Returns the compile-time graph batch number for this TU (2840).
 *   uint32_t __gj_batch_id_2840  (alias)
 *   __libcgj_batch2840_marker = "libcgj-batch2840"
 *
 * Milestone for the IPv4/endian exclusive wave (batches 2831-2839:
 * ipv4_ok_octet_u, ipv4_pack_u, ipv4_unpack_u, is_ipv4_loopback_u,
 * is_ipv4_private_u, is_ipv4_multicast_u, be16_load_u, be32_load_u,
 * le16_load_u). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2840_marker[] = "libcgj-batch2840";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2840_id(void)
{
	return 2840u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2840 - report this TU's graph batch number.
 *
 * Always returns 2840. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2840(void)
{
	(void)NULL;
	return b2840_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2840(void)
    __attribute__((alias("gj_batch_id_2840")));
