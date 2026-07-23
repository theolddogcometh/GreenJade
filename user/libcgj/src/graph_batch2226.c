/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2226: u32 open-address slot empty predicate.
 *
 * Surface (unique symbols):
 *   int gj_slot_empty_u32_p(uint32_t slot);
 *     - Return 1 if slot equals the empty sentinel 0xFFFFFFFFu, else 0.
 *       Used by open-addressed u32 tables to end a probe chain.
 *   int __gj_slot_empty_u32_p  (alias)
 *   __libcgj_batch2226_marker = "libcgj-batch2226"
 *
 * Post-2220 table exclusive wave (2221-2230). Unique
 * gj_slot_empty_u32_p surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2226_marker[] = "libcgj-batch2226";

/* Empty-slot sentinel shared by the post-2220 u32 table wave. */
#define B2226_EMPTY  0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2226_empty_p(uint32_t uSlot)
{
	return (uSlot == B2226_EMPTY) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slot_empty_u32_p - 1 iff slot is the open-address empty sentinel.
 *
 * slot: raw key-slot word from a u32 open-addressed table
 *
 * Returns 1 when slot == 0xFFFFFFFFu, else 0. Does not call libc.
 */
int
gj_slot_empty_u32_p(uint32_t slot)
{
	(void)NULL;
	return b2226_empty_p(slot);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_slot_empty_u32_p(uint32_t slot)
    __attribute__((alias("gj_slot_empty_u32_p")));
