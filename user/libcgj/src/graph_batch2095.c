/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2095: freestanding next u32 sequence step.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seq_next_u32(uint32_t s);
 *     - Return s + 1 with natural uint32_t wrap (UINT32_MAX -> 0).
 *   uint32_t __gj_seq_next_u32  (alias)
 *   __libcgj_batch2095_marker = "libcgj-batch2095"
 *
 * Post-2000 id exclusive wave (2091-2100). Distinct from
 * gj_gen_count_inc_sat (batch2093 saturates) - this wraps; unique
 * surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2095_marker[] = "libcgj-batch2095";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2095_next(uint32_t u32S)
{
	return u32S + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seq_next_u32 - modular successor of a 32-bit sequence number.
 *
 * s: current sequence value
 *
 * Returns s + 1 with unsigned wrap. Pair with gj_seq_gap_p (batch2094)
 * for drop detection. Does not call libc.
 */
uint32_t
gj_seq_next_u32(uint32_t u32S)
{
	(void)NULL;
	return b2095_next(u32S);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seq_next_u32(uint32_t u32S)
    __attribute__((alias("gj_seq_next_u32")));
