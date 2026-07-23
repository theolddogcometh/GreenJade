/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4769: slot table capacity bound check.
 *
 * Surface (unique symbols):
 *   int gj_slot_capacity_ok_u(uint32_t n, uint32_t max);
 *     - Return 1 if n <= max (requested count fits capacity), else 0.
 *   int __gj_slot_capacity_ok_u  (alias)
 *   __libcgj_batch4769_marker = "libcgj-batch4769"
 *
 * Exclusive continuum CREATE-ONLY (4761-4770: slot_is_free_u,
 * slot_is_live_u, slot_alloc_gen_u, slot_free_gen_u, slot_index_u,
 * slot_gen_u, slot_handle_u, slot_match_u, slot_capacity_ok_u,
 * batch_id_4770). Unique gj_slot_capacity_ok_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4769_marker[] = "libcgj-batch4769";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff requested count n is within capacity max. */
static int
b4769_capacity_ok(uint32_t u32N, uint32_t u32Max)
{
	return (u32N <= u32Max) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slot_capacity_ok_u - 1 if n fits in max capacity.
 *
 * n:   requested slot count / index+1 / live count
 * max: table capacity upper bound
 *
 * Returns 1 when n <= max, else 0. Self-contained; no parent wires.
 */
int
gj_slot_capacity_ok_u(uint32_t u32N, uint32_t u32Max)
{
	(void)NULL;
	return b4769_capacity_ok(u32N, u32Max);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_slot_capacity_ok_u(uint32_t u32N, uint32_t u32Max)
    __attribute__((alias("gj_slot_capacity_ok_u")));
