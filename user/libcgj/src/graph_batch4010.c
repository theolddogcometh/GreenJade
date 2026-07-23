/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4010: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4010(void);
 *     - Returns the compile-time graph batch number for this TU (4010).
 *   uint32_t __gj_batch_id_4010  (alias)
 *   __libcgj_batch4010_marker = "libcgj-batch4010"
 *
 * Exclusive continuum CREATE-ONLY (4001-4010: u32_clamp01_u, u64_clamp01_u,
 * u32_signbit_u, u64_signbit_u, u32_clear_msb_u, u64_clear_msb_u,
 * u32_set_msb_u, u64_set_msb_u, u32_toggle_msb_u, batch_id_4010).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4010_marker[] = "libcgj-batch4010";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4010_id(void)
{
	return 4010u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4010 - report this TU's graph batch number.
 *
 * Always returns 4010. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4010(void)
{
	(void)NULL;
	return b4010_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4010(void)
    __attribute__((alias("gj_batch_id_4010")));
