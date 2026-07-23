/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4490: wave batch identity (float-free
 * rational unique).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4490(void);
 *     - Returns the compile-time graph batch number for this TU (4490).
 *   uint32_t __gj_batch_id_4490  (alias)
 *   __libcgj_batch4490_marker = "libcgj-batch4490"
 *
 * Exclusive continuum CREATE-ONLY (4481-4490: mul_div_u2, scale_u,
 * u64_mul_div_u2, percent_u, basis_points_u, lerp_u, inv_lerp_u2,
 * smoothstep_tmax_u, clamp01_frac_u, batch_id_4490). Does NOT redefine
 * gj_batch_id / prior batch_id_* — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4490_marker[] = "libcgj-batch4490";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4490_id(void)
{
	return 4490u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4490 - report this TU's graph batch number.
 *
 * Always returns 4490.
 * Self-contained; no parent wires.
 */
uint32_t
gj_batch_id_4490(void)
{
	(void)NULL;
	return b4490_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4490(void)
    __attribute__((alias("gj_batch_id_4490")));
