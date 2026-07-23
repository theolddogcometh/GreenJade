/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4120: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4120(void);
 *     - Returns the compile-time graph batch number for this TU (4120).
 *   uint32_t __gj_batch_id_4120  (alias)
 *   __libcgj_batch4120_marker = "libcgj-batch4120"
 *
 * Exclusive continuum CREATE-ONLY (4111-4120: u32_rol_n_u, u32_ror_n_u,
 * u64_rol_n_u, u64_ror_n_u, u32_shl_sat_u, u32_shr_arith_u, u64_shl_sat_u,
 * u32_mask_low_n_u, u64_mask_low_n_u, batch_id_4120). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4120_marker[] = "libcgj-batch4120";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4120_id(void)
{
	return 4120u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4120 - report this TU's graph batch number.
 *
 * Always returns 4120. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4120(void)
{
	(void)NULL;
	return b4120_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4120(void)
    __attribute__((alias("gj_batch_id_4120")));
