/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4764: free a generation (clear live bit).
 *
 * Surface (unique symbols):
 *   uint32_t gj_slot_free_gen_u(uint32_t gen);
 *     - Clear the live bit: gen & ~1u. Result is always free/even.
 *   uint32_t __gj_slot_free_gen_u  (alias)
 *   __libcgj_batch4764_marker = "libcgj-batch4764"
 *
 * Exclusive continuum CREATE-ONLY (4761-4770: slot_is_free_u,
 * slot_is_live_u, slot_alloc_gen_u, slot_free_gen_u, slot_index_u,
 * slot_gen_u, slot_handle_u, slot_match_u, slot_capacity_ok_u,
 * batch_id_4770). Unique gj_slot_free_gen_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4764_marker[] = "libcgj-batch4764";

/* ---- freestanding helpers ---------------------------------------------- */

/* Clear live bit so generation is free/even. */
static uint32_t
b4764_free_gen(uint32_t u32Gen)
{
	return u32Gen & ~1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slot_free_gen_u - clear live bit on a generation counter.
 *
 * gen: prior free or live generation
 *
 * Returns gen & ~1u (always even / free). Self-contained; no parent wires.
 */
uint32_t
gj_slot_free_gen_u(uint32_t u32Gen)
{
	(void)NULL;
	return b4764_free_gen(u32Gen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_slot_free_gen_u(uint32_t u32Gen)
    __attribute__((alias("gj_slot_free_gen_u")));
