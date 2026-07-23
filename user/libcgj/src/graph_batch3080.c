/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3080: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3080(void);
 *     - Returns the compile-time graph batch number for this TU (3080).
 *   uint32_t __gj_batch_id_3080  (alias)
 *   __libcgj_batch3080_marker = "libcgj-batch3080"
 *
 * Exclusive continuum CREATE-ONLY (3071-3080): tan_q15_u, atan2_q15_u,
 * hypot_u32_u, hypot_u64_u, dist2_u32_u, dist2_u64_u, manhattan_u32_u,
 * manhattan_u64_u, chebyshev_u32_u, batch_id_3080. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3080_marker[] = "libcgj-batch3080";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3080_id(void)
{
	return 3080u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3080 - report this TU's graph batch number.
 *
 * Always returns 3080.
 */
uint32_t
gj_batch_id_3080(void)
{
	(void)NULL;
	return b3080_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3080(void)
    __attribute__((alias("gj_batch_id_3080")));
