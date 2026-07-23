/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14384: bar3 deck checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_deck_check_u_14384(void);
 *     - Returns 0 (soft stub: deck checklist slot is soft/unprobed;
 *       not a runtime Steam Deck / deck-path probe).
 *   uint32_t __gj_bar3_deck_check_u_14384  (alias)
 *   __libcgj_batch14384_marker = "libcgj-batch14384"
 *
 * Exclusive continuum CREATE-ONLY (14381-14390: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_deck_check_u_14384 surface only; no
 * multi-def. Distinct from gj_bar3_deck_check_u_13884,
 * gj_bar3_deck_check_u_13684, gj_bar3_deck_bit (batch2499), and
 * sibling check_u soft stubs in this wave. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14384_marker[] = "libcgj-batch14384";

/* Soft-stub deck checklist value (unprobed). */
#define B14384_DECK_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14384_deck_check(void)
{
	return B14384_DECK_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_deck_check_u_14384 - soft bar3 deck checklist stub.
 *
 * Always returns 0 (soft/unprobed deck slot). Does not call libc or
 * probe deck paths. No parent wires.
 */
uint32_t
gj_bar3_deck_check_u_14384(void)
{
	(void)NULL;
	return b14384_deck_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_deck_check_u_14384(void)
    __attribute__((alias("gj_bar3_deck_check_u_14384")));
