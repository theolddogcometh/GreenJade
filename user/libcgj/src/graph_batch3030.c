/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3030: batch identity (closes exclusive
 * continuum arith wave 3021-3030).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3030(void);
 *     - Returns the compile-time graph batch number for this TU (3030).
 *   uint32_t __gj_batch_id_3030  (alias)
 *   __libcgj_batch3030_marker = "libcgj-batch3030"
 *
 * Exclusive continuum CREATE-ONLY wave (3021-3030:
 * gj_borrow_sub_u32_u, gj_borrow_sub_u64_u, gj_u32_inc_sat_u,
 * gj_u32_dec_sat_u, gj_u64_inc_sat_u, gj_u64_dec_sat_u,
 * gj_u32_neg_mod_u, gj_u64_neg_mod_u, gj_u32_twice_sat_u).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3030_marker[] = "libcgj-batch3030";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3030_id(void)
{
	return 3030u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3030 - report this TU's graph batch number.
 *
 * Always returns 3030. No parent wires.
 */
uint32_t
gj_batch_id_3030(void)
{
	(void)NULL;
	return b3030_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3030(void)
    __attribute__((alias("gj_batch_id_3030")));
