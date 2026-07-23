/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2950: milestone 2950 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2950(void);
 *     — Returns the compile-time graph batch number for this TU (2950).
 *   uint32_t gj_graph_milestone_2950(void);
 *     — Returns the current graph milestone revision (2950).
 *   uint32_t __gj_batch_id_2950  (alias)
 *   uint32_t __gj_graph_milestone_2950  (alias)
 *   __libcgj_batch2950_marker = "libcgj-batch2950"
 *
 * Milestone for the exclusive modular/number-theory continuum wave
 * (batches 2941–2949: u64_sub_mod_u, u64_mul_mod_u, pow_mod_u32_u,
 * inv_mod_u32_u, is_prime_u32_u, next_prime_u32_u, fib_u32_u, fib_u64_u,
 * fact_u32_u). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* or milestone_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2950_marker[] = "libcgj-batch2950";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2950_id(void)
{
	return 2950u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2950 — report this TU's graph batch number.
 *
 * Always returns 2950.
 */
uint32_t
gj_batch_id_2950(void)
{
	(void)NULL;
	return b2950_id();
}

/*
 * gj_graph_milestone_2950 — report this TU's graph milestone revision.
 *
 * Always returns 2950 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2950(void)
{
	return b2950_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2950(void)
    __attribute__((alias("gj_batch_id_2950")));

uint32_t __gj_graph_milestone_2950(void)
    __attribute__((alias("gj_graph_milestone_2950")));
