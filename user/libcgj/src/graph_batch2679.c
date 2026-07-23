/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2679: product bar3 set Top50 bit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_set_top50(uint32_t mask);
 *     - Return mask with the Deck Top 50 readiness bit set
 *       (bit2 | 0x4). Soft pure-data update; does not claim matrix
 *       title PASS.
 *   uint32_t __gj_bar3_set_top50  (alias)
 *   __libcgj_batch2679_marker = "libcgj-batch2679"
 *
 * Milestone 2680 exclusive product bar3 bitmask helpers (2671-2680).
 * Unique gj_bar3_set_top50 surface only; no multi-def. Pair with
 * gj_bar3_top50_ready_p (batch2674) and gj_bar3_all_ready_p
 * (batch2675).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2679_marker[] = "libcgj-batch2679";

/* Deck Top 50 readiness bit within product bar3 mask (bit2). */
#define B2679_TOP50_BIT  0x4u

/* ---- freestanding helpers ---------------------------------------------- */

/* Set Top50 readiness bit in mask. */
static uint32_t
b2679_set_top50(uint32_t uMask)
{
	return uMask | B2679_TOP50_BIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_set_top50 - set Top50 readiness bit on bar3 mask.
 *
 * mask: product bar3 readiness bitmask
 *
 * Returns mask | 0x4. Idempotent if bit2 already set. Does not call
 * libc.
 */
uint32_t
gj_bar3_set_top50(uint32_t uMask)
{
	(void)NULL;
	return b2679_set_top50(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_set_top50(uint32_t uMask)
    __attribute__((alias("gj_bar3_set_top50")));
