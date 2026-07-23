/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2040: milestone 2040 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2040(void);
 *     — Returns the compile-time graph batch number for this TU (2040).
 *   uint32_t gj_graph_milestone_2040(void);
 *     — Returns the current graph milestone revision (2040).
 *   uint32_t __gj_batch_id_2040  (alias)
 *   uint32_t __gj_graph_milestone_2040  (alias)
 *   __libcgj_batch2040_marker = "libcgj-batch2040"
 *
 * Milestone for the post-2030 u64 arith exclusive wave (batches
 * 2031–2039: add_sat, sub_sat, mul_sat_soft, div_floor, div_ceil_u_soft,
 * mod_u, gcd_euclid, lcm_sat, pow_sat). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols — avoid
 * multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2040_marker[] = "libcgj-batch2040";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2040_id(void)
{
	return 2040u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2040 — report this TU's graph batch number.
 *
 * Always returns 2040.
 */
uint32_t
gj_batch_id_2040(void)
{
	(void)NULL;
	return b2040_id();
}

/*
 * gj_graph_milestone_2040 — report this TU's graph milestone revision.
 *
 * Always returns 2040 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2040(void)
{
	return b2040_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2040(void)
    __attribute__((alias("gj_batch_id_2040")));

uint32_t __gj_graph_milestone_2040(void)
    __attribute__((alias("gj_graph_milestone_2040")));
