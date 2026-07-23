/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4130: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4130(void);
 *     - Returns the compile-time graph batch number for this TU (4130).
 *   uint32_t __gj_batch_id_4130  (alias)
 *   __libcgj_batch4130_marker = "libcgj-batch4130"
 *
 * Exclusive continuum CREATE-ONLY (4121-4130: u32_bf_extract_u,
 * u64_bf_extract_u, u32_bf_deposit_u, u64_bf_deposit_u, u32_bf_clear_u,
 * u64_bf_clear_u, u32_bf_set_all_u, u64_bf_set_all_u, u32_bf_width_mask_u,
 * batch_id_4130).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4130_marker[] = "libcgj-batch4130";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4130_id(void)
{
	return 4130u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4130 - report this TU's graph batch number.
 *
 * Always returns 4130. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4130(void)
{
	(void)NULL;
	return b4130_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4130(void)
    __attribute__((alias("gj_batch_id_4130")));
