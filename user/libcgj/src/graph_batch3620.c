/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3620: batch identity (closes exclusive
 * continuum arith wave 3611-3620).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3620(void);
 *     - Returns the compile-time graph batch number for this TU (3620).
 *   uint32_t __gj_batch_id_3620  (alias)
 *   __libcgj_batch3620_marker = "libcgj-batch3620"
 *
 * Exclusive continuum CREATE-ONLY wave (3611-3620:
 * gj_u64_sub_with_borrow_u, gj_u32_widening_mul_lo_u,
 * gj_u32_widening_mul_hi_u, gj_u64_mul_hi_u, gj_u32_div_rem_u,
 * gj_u64_div_rem_u, gj_u32_checked_div_u, gj_u64_checked_div_u,
 * gj_u32_checked_mod_u, batch_id_3620). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols - avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3620_marker[] = "libcgj-batch3620";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3620_id(void)
{
	return 3620u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3620 - report this TU's graph batch number.
 *
 * Always returns 3620. No parent wires.
 */
uint32_t
gj_batch_id_3620(void)
{
	(void)NULL;
	return b3620_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3620(void)
    __attribute__((alias("gj_batch_id_3620")));
