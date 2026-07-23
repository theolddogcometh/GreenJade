/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4470: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4470(void);
 *     - Returns the compile-time graph batch number for this TU (4470).
 *   uint32_t __gj_batch_id_4470  (alias)
 *   __libcgj_batch4470_marker = "libcgj-batch4470"
 *
 * Exclusive continuum CREATE-ONLY (4461-4470: ip_hdr_checksum_u,
 * udp_pseudo_fold_u, csum_add_u, csum_fold_u, csum_finish_u, htons_u,
 * ntohs_u, htonl_u, ntohl_u, batch_id_4470). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4470_marker[] = "libcgj-batch4470";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4470_id(void)
{
	return 4470u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4470 - report this TU's graph batch number.
 *
 * Always returns 4470. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4470(void)
{
	(void)NULL;
	return b4470_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4470(void)
    __attribute__((alias("gj_batch_id_4470")));
