/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2780: milestone 2780 batch identity
 * (exclusive continuum wave 2771-2780).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2780(void);
 *     - Returns the compile-time graph batch number for this TU (2780).
 *   uint32_t __gj_batch_id_2780  (alias)
 *   __libcgj_batch2780_marker = "libcgj-batch2780"
 *
 * Milestone for the exclusive integer helpers wave (batches 2771-2779:
 * i64_sign_u, u32/u64 is_pow2_u, u32/u64 next_pow2_u, gcd/lcm u32/u64
 * _u). Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2780_marker[] = "libcgj-batch2780";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2780_id(void)
{
	return 2780u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2780 - report this TU's graph batch number.
 *
 * Always returns 2780. Link-time presence of this symbol tags the wave.
 * Does not call libc.
 */
uint32_t
gj_batch_id_2780(void)
{
	(void)NULL;
	return b2780_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2780(void)
    __attribute__((alias("gj_batch_id_2780")));
