/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4763: allocate/advance generation to live.
 *
 * Surface (unique symbols):
 *   uint32_t gj_slot_alloc_gen_u(uint32_t gen);
 *     - Force gen into the live (odd) domain: gen | 1. Equivalent to
 *       gen + 1 when gen was free/even. Does not bump an already-live
 *       generation past its odd value.
 *   uint32_t __gj_slot_alloc_gen_u  (alias)
 *   __libcgj_batch4763_marker = "libcgj-batch4763"
 *
 * Exclusive continuum CREATE-ONLY (4761-4770: slot_is_free_u,
 * slot_is_live_u, slot_alloc_gen_u, slot_free_gen_u, slot_index_u,
 * slot_gen_u, slot_handle_u, slot_match_u, slot_capacity_ok_u,
 * batch_id_4770). Unique gj_slot_alloc_gen_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4763_marker[] = "libcgj-batch4763";

/* ---- freestanding helpers ---------------------------------------------- */

/* Set live bit (LSB) so generation is odd. */
static uint32_t
b4763_alloc_gen(uint32_t u32Gen)
{
	return u32Gen | 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slot_alloc_gen_u - produce a live (odd) generation from gen.
 *
 * gen: prior free or live generation
 *
 * Returns gen | 1 (always odd / live). When gen was free/even this is
 * gen + 1. Self-contained; no parent wires.
 */
uint32_t
gj_slot_alloc_gen_u(uint32_t u32Gen)
{
	(void)NULL;
	return b4763_alloc_gen(u32Gen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_slot_alloc_gen_u(uint32_t u32Gen)
    __attribute__((alias("gj_slot_alloc_gen_u")));
