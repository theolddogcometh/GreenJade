/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3310: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3310(void);
 *     - Returns the compile-time graph batch number for this TU (3310).
 *   uint32_t __gj_batch_id_3310  (alias)
 *   __libcgj_batch3310_marker = "libcgj-batch3310"
 *
 * Exclusive continuum CREATE-ONLY (3301-3310): u32_clog2_u, u64_clog2_u,
 * u32_flog2_u, u64_flog2_u, u32_isqrt_ceil_u, u64_isqrt_ceil_u,
 * u32_cbrt_floor_u, u64_cbrt_floor_u, u32_pow_u, batch_id_3310.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3310_marker[] = "libcgj-batch3310";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3310_id(void)
{
	return 3310u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3310 - report this TU's graph batch number.
 *
 * Always returns 3310.
 */
uint32_t
gj_batch_id_3310(void)
{
	(void)NULL;
	return b3310_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3310(void)
    __attribute__((alias("gj_batch_id_3310")));
