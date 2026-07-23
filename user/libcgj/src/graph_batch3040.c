/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3040: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3040(void);
 *     - Returns the compile-time graph batch number for this TU (3040).
 *   uint32_t __gj_batch_id_3040  (alias)
 *   __libcgj_batch3040_marker = "libcgj-batch3040"
 *
 * CREATE-ONLY exclusive continuum (3031-3040): u64_twice_sat_u,
 * u32_half_ceil_u, u64_half_ceil_u, u32_half_floor_u, u64_half_floor_u,
 * sign_extend_u8_u, sign_extend_u16_u, zero_extend_u8_u,
 * zero_extend_u16_u, batch_id_3040. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3040_marker[] = "libcgj-batch3040";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3040_id(void)
{
	return 3040u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3040 - report this TU's graph batch number.
 *
 * Always returns 3040.
 */
uint32_t
gj_batch_id_3040(void)
{
	(void)NULL;
	return b3040_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3040(void)
    __attribute__((alias("gj_batch_id_3040")));
