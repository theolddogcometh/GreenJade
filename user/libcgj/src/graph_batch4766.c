/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4766: extract generation from packed handle.
 *
 * Surface (unique symbols):
 *   uint32_t gj_slot_gen_u(uint64_t handle);
 *     - Return the high 32 bits of handle (generation field).
 *   uint32_t __gj_slot_gen_u  (alias)
 *   __libcgj_batch4766_marker = "libcgj-batch4766"
 *
 * Exclusive continuum CREATE-ONLY (4761-4770: slot_is_free_u,
 * slot_is_live_u, slot_alloc_gen_u, slot_free_gen_u, slot_index_u,
 * slot_gen_u, slot_handle_u, slot_match_u, slot_capacity_ok_u,
 * batch_id_4770). Unique gj_slot_gen_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4766_marker[] = "libcgj-batch4766";

/* ---- freestanding helpers ---------------------------------------------- */

/* High 32 bits of a packed (gen:index) handle. */
static uint32_t
b4766_gen(uint64_t u64Handle)
{
	return (uint32_t)(u64Handle >> 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slot_gen_u - extract generation (high 32) from a packed handle.
 *
 * handle: packed uint64_t (high=gen, low=index)
 *
 * Returns (uint32_t)(handle >> 32). Self-contained; no parent wires.
 */
uint32_t
gj_slot_gen_u(uint64_t u64Handle)
{
	(void)NULL;
	return b4766_gen(u64Handle);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_slot_gen_u(uint64_t u64Handle)
    __attribute__((alias("gj_slot_gen_u")));
