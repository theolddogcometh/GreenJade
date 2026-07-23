/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4767: pack index+gen into a slot handle.
 *
 * Surface (unique symbols):
 *   uint64_t gj_slot_handle_u(uint32_t index, uint32_t gen);
 *     - Pack ((uint64_t)gen << 32) | index into a handle.
 *   uint64_t __gj_slot_handle_u  (alias)
 *   __libcgj_batch4767_marker = "libcgj-batch4767"
 *
 * Exclusive continuum CREATE-ONLY (4761-4770: slot_is_free_u,
 * slot_is_live_u, slot_alloc_gen_u, slot_free_gen_u, slot_index_u,
 * slot_gen_u, slot_handle_u, slot_match_u, slot_capacity_ok_u,
 * batch_id_4770). Unique gj_slot_handle_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4767_marker[] = "libcgj-batch4767";

/* ---- freestanding helpers ---------------------------------------------- */

/* Pack generation (high) and index (low) into a 64-bit handle. */
static uint64_t
b4767_handle(uint32_t u32Index, uint32_t u32Gen)
{
	return ((uint64_t)u32Gen << 32) | (uint64_t)u32Index;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slot_handle_u - pack slot index and generation into a handle.
 *
 * index: low 32-bit slot index
 * gen:   high 32-bit generation (including live bit)
 *
 * Returns ((uint64_t)gen << 32) | index. Self-contained; no parent wires.
 */
uint64_t
gj_slot_handle_u(uint32_t u32Index, uint32_t u32Gen)
{
	(void)NULL;
	return b4767_handle(u32Index, u32Gen);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_slot_handle_u(uint32_t u32Index, uint32_t u32Gen)
    __attribute__((alias("gj_slot_handle_u")));
