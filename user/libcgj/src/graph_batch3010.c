/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3010: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3010(void);
 *     - Returns the compile-time graph batch number for this TU (3010).
 *   uint32_t __gj_batch_id_3010  (alias)
 *   __libcgj_batch3010_marker = "libcgj-batch3010"
 *
 * Exclusive continuum CREATE-ONLY (3001-3010): log2_ceil_u u32/u64,
 * div_floor_u u32/u64, rem_u u32/u64, mul_hi_u, mul_hi_portable_u,
 * avg_floor_u, batch_id_3010. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3010_marker[] = "libcgj-batch3010";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3010_id(void)
{
	return 3010u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3010 - report this TU's graph batch number.
 *
 * Always returns 3010. No parent wires.
 */
uint32_t
gj_batch_id_3010(void)
{
	(void)NULL;
	return b3010_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3010(void)
    __attribute__((alias("gj_batch_id_3010")));
