/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2227: u32 open-address slot tombstone predicate.
 *
 * Surface (unique symbols):
 *   int gj_slot_tomb_u32_p(uint32_t slot);
 *     - Return 1 if slot equals the tombstone sentinel 0xFFFFFFFEu,
 *       else 0. Tombstones are skippable during lookup and reclaimable
 *       during insert; they keep probe chains intact after delete.
 *   int __gj_slot_tomb_u32_p  (alias)
 *   __libcgj_batch2227_marker = "libcgj-batch2227"
 *
 * Post-2220 table exclusive wave (2221-2230). Unique
 * gj_slot_tomb_u32_p surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2227_marker[] = "libcgj-batch2227";

/* Tombstone-slot sentinel shared by the post-2220 u32 table wave. */
#define B2227_TOMB  0xFFFFFFFEu

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2227_tomb_p(uint32_t uSlot)
{
	return (uSlot == B2227_TOMB) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slot_tomb_u32_p - 1 iff slot is the open-address tombstone sentinel.
 *
 * slot: raw key-slot word from a u32 open-addressed table
 *
 * Returns 1 when slot == 0xFFFFFFFEu, else 0. Does not call libc.
 */
int
gj_slot_tomb_u32_p(uint32_t slot)
{
	(void)NULL;
	return b2227_tomb_p(slot);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_slot_tomb_u32_p(uint32_t slot)
    __attribute__((alias("gj_slot_tomb_u32_p")));
