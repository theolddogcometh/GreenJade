/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4040: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4040(void);
 *     - Returns the compile-time graph batch number for this TU (4040).
 *   uint32_t __gj_batch_id_4040  (alias)
 *   __libcgj_batch4040_marker = "libcgj-batch4040"
 *
 * Exclusive continuum CREATE-ONLY (4031-4040): u64_scramble_u,
 * u32_permute_bits_u, u64_permute_bits_u, u32_unpermute_bits_u,
 * u64_unpermute_bits_u, u32_spread_bits_u, u64_spread_bits_u,
 * u32_compact_bits_u, u64_compact_bits_u, batch_id_4040. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4040_marker[] = "libcgj-batch4040";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4040_id(void)
{
	return 4040u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4040 - report this TU's graph batch number.
 *
 * Always returns 4040.
 */
uint32_t
gj_batch_id_4040(void)
{
	(void)NULL;
	return b4040_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4040(void)
    __attribute__((alias("gj_batch_id_4040")));
