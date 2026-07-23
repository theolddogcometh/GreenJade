/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3198: product bar3 readiness score (3200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_score_3200(uint32_t mask);
 *     - Return the number of set bits among the low three product bar3
 *       slots of mask (popcount of mask & 0x7), range 0..3. Soft
 *       pure-data score for Steam/Deck/Top50 completeness on the
 *       milestone 3200 continuum.
 *   uint32_t __gj_bar3_score_3200  (alias)
 *   __libcgj_batch3198_marker = "libcgj-batch3198"
 *
 * Milestone 3200 exclusive continuum CREATE-ONLY (3191-3200). Unique
 * gj_bar3_score_3200 surface only; no multi-def. Distinct from
 * gj_bar3_score_u (batch2699), gj_bar3_partial_u (batch3099),
 * gj_bar3_progress_u (batch2997), and gj_product_bar3_mask
 * (batch2597). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3198_marker[] = "libcgj-batch3198";

/* Three product bar3 readiness bits (bits 0..2). */
#define B3198_BAR3_MASK  0x7u

/* ---- freestanding helpers ---------------------------------------------- */

/* Popcount of low three bar3 bits (0..3). */
static uint32_t
b3198_score(uint32_t uMask)
{
uint32_t uBits;
uint32_t uScore;

uBits = uMask & B3198_BAR3_MASK;
uScore = 0u;
if ((uBits & 0x1u) != 0u) {
uScore = uScore + 1u;
}
if ((uBits & 0x2u) != 0u) {
uScore = uScore + 1u;
}
if ((uBits & 0x4u) != 0u) {
uScore = uScore + 1u;
}
return uScore;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_score_3200 - count set bar3 readiness slots in mask.
 *
 * mask: product bar3 readiness bitmask (bits 0..2 = Steam/Deck/Top50)
 *
 * Returns popcount of (mask & 0x7), always in 0..3. Soft pure-data
 * only; does not probe Steam/Deck. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_score_3200(uint32_t uMask)
{
(void)NULL;
return b3198_score(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_score_3200(uint32_t uMask)
    __attribute__((alias("gj_bar3_score_3200")));
