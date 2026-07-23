/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5825: bar3 top50 mask verify score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_top50_mask_verify_score_5825(uint32_t mask);
 *     - Stub verify score from low three bar3 bits (steam/deck/top50):
 *       each set bit adds 25, base 0, max 75. Soft pure-data only.
 *   uint32_t __gj_bar3_top50_mask_verify_score_5825  (alias)
 *   __libcgj_batch5825_marker = "libcgj-batch5825"
 *
 * Exclusive continuum CREATE-ONLY (5821-5830: product bar3 top50
 * verify finalize). Unique gj_bar3_top50_mask_verify_score_5825
 * surface only; no multi-def. Distinct from
 * gj_bar3_title_mask_score_5725 (batch5725). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5825_marker[] = "libcgj-batch5825";

/* steam | deck | top50 (bits 0..2) */
#define B5825_BAR3_MASK   0x7u
#define B5825_SCORE_STEP  25u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5825_pop3(uint32_t uMask)
{
uint32_t uN;
uint32_t uBits;

uN = 0u;
uBits = uMask & B5825_BAR3_MASK;
while (uBits != 0u) {
uN += (uBits & 1u);
uBits >>= 1;
}
return uN;
}

static uint32_t
b5825_mask_score(uint32_t uMask)
{
return B5825_SCORE_STEP * b5825_pop3(uMask);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_top50_mask_verify_score_5825 - stub verify score from bar3 bits.
 *
 * mask: product bar3 readiness bitmask (low three bits used).
 * Returns 0, 25, 50, or 75 depending on popcount of bits 0..2.
 * Does not probe product bars. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_top50_mask_verify_score_5825(uint32_t uMask)
{
(void)NULL;
return b5825_mask_score(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_top50_mask_verify_score_5825(uint32_t uMask)
    __attribute__((alias("gj_bar3_top50_mask_verify_score_5825")));
