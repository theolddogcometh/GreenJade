/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4768: match handle generation against gen.
 *
 * Surface (unique symbols):
 *   int gj_slot_match_u(uint64_t handle, uint32_t gen);
 *     - Return 1 if the high-32 generation field of handle equals gen,
 *       else 0. Used to detect ABA/stale handles after free+realloc.
 *   int __gj_slot_match_u  (alias)
 *   __libcgj_batch4768_marker = "libcgj-batch4768"
 *
 * Exclusive continuum CREATE-ONLY (4761-4770: slot_is_free_u,
 * slot_is_live_u, slot_alloc_gen_u, slot_free_gen_u, slot_index_u,
 * slot_gen_u, slot_handle_u, slot_match_u, slot_capacity_ok_u,
 * batch_id_4770). Unique gj_slot_match_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4768_marker[] = "libcgj-batch4768";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff handle high-32 generation equals expected gen. */
static int
b4768_match(uint64_t u64Handle, uint32_t u32Gen)
{
	uint32_t u32HGen;

	u32HGen = (uint32_t)(u64Handle >> 32);
	return (u32HGen == u32Gen) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slot_match_u - 1 if handle's packed generation equals gen.
 *
 * handle: packed uint64_t (high=gen, low=index)
 * gen:    expected generation
 *
 * Returns 1 on exact high-32 match, else 0. Self-contained; no parent
 * wires.
 */
int
gj_slot_match_u(uint64_t u64Handle, uint32_t u32Gen)
{
	(void)NULL;
	return b4768_match(u64Handle, u32Gen);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_slot_match_u(uint64_t u64Handle, uint32_t u32Gen)
    __attribute__((alias("gj_slot_match_u")));
