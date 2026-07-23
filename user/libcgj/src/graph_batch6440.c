/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6440: exclusive DLC/depot manifest size batch
 * identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6440(void);
 *     - Returns the compile-time graph batch number for this TU (6440).
 *   uint32_t __gj_batch_id_6440  (alias)
 *   __libcgj_batch6440_marker = "libcgj-batch6440"
 *
 * Exclusive continuum CREATE-ONLY (6431-6440: DLC/depot manifest size
 * stubs — dlc_id_ok_6431, depot_id_ok_6432, manifest_size_ok_6433,
 * dlc_manifest_hdr_size_6434, depot_manifest_max_size_6435,
 * depot_chunk_size_ok_6436, manifest_entry_count_ok_6437,
 * dlc_size_pack_6438, depot_manifest_size_gate_6439, batch_id_6440).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6440_marker[] = "libcgj-batch6440";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6440_id(void)
{
	return 6440u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6440 - report this TU's graph batch number.
 *
 * Always returns 6440. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_6440(void)
{
	(void)NULL;
	return b6440_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6440(void)
    __attribute__((alias("gj_batch_id_6440")));
