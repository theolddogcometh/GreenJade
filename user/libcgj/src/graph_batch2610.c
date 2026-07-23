/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2610: milestone 2610 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2610(void);
 *     - Returns the compile-time graph batch number for this TU (2610).
 *   uint32_t __gj_batch_id_2610  (alias)
 *   __libcgj_batch2610_marker = "libcgj-batch2610"
 *
 * Milestone for the exclusive min/max/clamp/absdiff wave (batches
 * 2601-2609: u64_min_u, u64_max_u, u32_min_u, u32_max_u, i64_min_u,
 * i64_max_u, u64_clamp_u, i64_clamp_u, u64_absdiff_u). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2610_marker[] = "libcgj-batch2610";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2610_id(void)
{
	return 2610u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2610 — report this TU's graph batch number.
 *
 * Always returns 2610. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2610(void)
{
	(void)NULL;
	return b2610_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2610(void)
    __attribute__((alias("gj_batch_id_2610")));
