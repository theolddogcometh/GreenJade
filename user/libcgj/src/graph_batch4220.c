/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4220: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4220(void);
 *     - Returns the compile-time graph batch number for this TU (4220).
 *   uint32_t __gj_batch_id_4220  (alias)
 *   __libcgj_batch4220_marker = "libcgj-batch4220"
 *
 * Exclusive continuum CREATE-ONLY (4211-4220: u32_min3_u, u32_max3_u,
 * u64_min3_u, u64_max3_u, u32_clamp_range_u, u64_clamp_range_u,
 * i32_clamp_range_s, i32_abs_s, i32_sign_s, batch_id_4220).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4220_marker[] = "libcgj-batch4220";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4220_id(void)
{
	return 4220u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4220 - report this TU's graph batch number.
 *
 * Always returns 4220.
 */
uint32_t
gj_batch_id_4220(void)
{
	(void)NULL;
	return b4220_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4220(void)
    __attribute__((alias("gj_batch_id_4220")));
