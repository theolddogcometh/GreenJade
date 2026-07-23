/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4310: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4310(void);
 *     - Returns the compile-time graph batch number for this TU (4310).
 *   uint32_t __gj_batch_id_4310  (alias)
 *   __libcgj_batch4310_marker = "libcgj-batch4310"
 *
 * Exclusive continuum CREATE-ONLY (4301-4310: u32_shl_checked_u,
 * u32_shr_checked_u, u64_shl_checked_u, u64_shr_checked_u, u32_shl_fill_u,
 * u64_shl_fill_u, u32_bit_test_u, u64_bit_test_u, u32_bit_set_u,
 * batch_id_4310). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4310_marker[] = "libcgj-batch4310";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4310_id(void)
{
	return 4310u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4310 - report this TU's graph batch number.
 *
 * Always returns 4310. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4310(void)
{
	(void)NULL;
	return b4310_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4310(void)
    __attribute__((alias("gj_batch_id_4310")));
