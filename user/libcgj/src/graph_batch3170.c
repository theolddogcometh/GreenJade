/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3170: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3170(void);
 *     - Returns the compile-time graph batch number for this TU (3170).
 *   uint32_t __gj_batch_id_3170  (alias)
 *   __libcgj_batch3170_marker = "libcgj-batch3170"
 *
 * Exclusive continuum CREATE-ONLY (3161-3170): range_contains_u,
 * range_union_need_u, range_intersect_p_u, bitset8_test/set/clr_u,
 * bitset16_test/set/clr_u, batch_id_3170. Does NOT redefine gj_batch_id
 * / gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3170_marker[] = "libcgj-batch3170";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3170_id(void)
{
	return 3170u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3170 - report this TU's graph batch number.
 *
 * Always returns 3170. No parent wires.
 */
uint32_t
gj_batch_id_3170(void)
{
	(void)NULL;
	return b3170_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3170(void)
    __attribute__((alias("gj_batch_id_3170")));
