/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3830: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3830(void);
 *     - Returns the compile-time graph batch number for this TU (3830).
 *   uint32_t __gj_batch_id_3830  (alias)
 *   __libcgj_batch3830_marker = "libcgj-batch3830"
 *
 * Exclusive continuum CREATE-ONLY (3821-3830): u8_pack_be_u,
 * u8_pack_le_u, u16_from_be_u, u16_from_le_u, u32_from_be_u,
 * u32_from_le_u, u64_from_be_u, u64_from_le_u, u16_to_be_u,
 * batch_id_3830. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3830_marker[] = "libcgj-batch3830";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3830_id(void)
{
	return 3830u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3830 - report this TU's graph batch number.
 *
 * Always returns 3830.
 */
uint32_t
gj_batch_id_3830(void)
{
	(void)NULL;
	return b3830_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3830(void)
    __attribute__((alias("gj_batch_id_3830")));
