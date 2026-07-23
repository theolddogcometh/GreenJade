/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4140: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4140(void);
 *     - Returns the compile-time graph batch number for this TU (4140).
 *   uint32_t __gj_batch_id_4140  (alias)
 *   __libcgj_batch4140_marker = "libcgj-batch4140"
 *
 * Exclusive continuum CREATE-ONLY (4131-4140: u16_load_be_u, u16_load_le_u,
 * u32_load_be_u, u32_load_le_u, u64_load_be_u, u64_load_le_u,
 * u32_store_be_u, u32_store_le_u, u16_bswap_u, batch_id_4140).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4140_marker[] = "libcgj-batch4140";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4140_id(void)
{
	return 4140u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4140 - report this TU's graph batch number.
 *
 * Always returns 4140. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4140(void)
{
	(void)NULL;
	return b4140_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4140(void)
    __attribute__((alias("gj_batch_id_4140")));
