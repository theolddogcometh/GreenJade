/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4762: generational slot live predicate.
 *
 * Surface (unique symbols):
 *   int gj_slot_is_live_u(uint32_t gen);
 *     - Return 1 if gen is live (odd: (gen & 1) != 0), else 0.
 *       Free slots hold an even generation (LSB clear).
 *   int __gj_slot_is_live_u  (alias)
 *   __libcgj_batch4762_marker = "libcgj-batch4762"
 *
 * Exclusive continuum CREATE-ONLY (4761-4770: slot_is_free_u,
 * slot_is_live_u, slot_alloc_gen_u, slot_free_gen_u, slot_index_u,
 * slot_gen_u, slot_handle_u, slot_match_u, slot_capacity_ok_u,
 * batch_id_4770). Distinct from gj_slot_empty_u32_p (batch2226) —
 * unique gj_slot_is_live_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4762_marker[] = "libcgj-batch4762";

/* ---- freestanding helpers ---------------------------------------------- */

/* Live iff generation LSB is set (odd gen). */
static int
b4762_is_live(uint32_t u32Gen)
{
	return ((u32Gen & 1u) != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slot_is_live_u - 1 if slot generation is live (odd).
 *
 * gen: slot generation counter (LSB is live bit)
 *
 * Returns 1 when (gen & 1) != 0, else 0. Self-contained; no parent wires.
 */
int
gj_slot_is_live_u(uint32_t u32Gen)
{
	(void)NULL;
	return b4762_is_live(u32Gen);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_slot_is_live_u(uint32_t u32Gen)
    __attribute__((alias("gj_slot_is_live_u")));
