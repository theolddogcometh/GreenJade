/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5105: simple soft IRQ priority from a vector.
 *
 * Surface (unique symbols):
 *   uint32_t gj_irq_priority_u(uint32_t v);
 *     - Simple soft priority: identity of v when v < 256, else 255.
 *       Lower value means higher urgency in this model.
 *   uint32_t __gj_irq_priority_u  (alias)
 *   __libcgj_batch5105_marker = "libcgj-batch5105"
 *
 * Exclusive continuum CREATE-ONLY (5101-5110). Distinct from
 * gj_job_priority_u (batch4771) and gj_job_priority_clamp (batch1309)
 * — unique gj_irq_priority_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5105_marker[] = "libcgj-batch5105";

/* Soft 8-bit vector space exclusive upper bound. */
#define B5105_VEC_LIMIT  256u
/* Priority assigned to out-of-range vectors (lowest urgency here). */
#define B5105_PRIO_MAX   255u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5105_priority(uint32_t u32V)
{
	if (u32V < B5105_VEC_LIMIT) {
		return u32V;
	}
	return B5105_PRIO_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_irq_priority_u - simple soft priority derived from vector v.
 *
 * v: interrupt / exception vector
 *
 * Returns v when v < 256 (identity priority: lower vector = higher
 * urgency), else 255. Pure integer map; no parent wires.
 */
uint32_t
gj_irq_priority_u(uint32_t u32V)
{
	(void)NULL;
	return b5105_priority(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_irq_priority_u(uint32_t u32V)
    __attribute__((alias("gj_irq_priority_u")));
