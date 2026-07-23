/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5828: product top50 verify gate predicate.
 *
 * Surface (unique symbols):
 *   int gj_top50_verify_p_5828(uint32_t score);
 *     - Return 1 if score is in [70, 100] (soft product top50 verify
 *       band for bar3 Top50 titles), else 0. Pure-data lamp only.
 *   int __gj_top50_verify_p_5828  (alias)
 *   __libcgj_batch5828_marker = "libcgj-batch5828"
 *
 * Exclusive continuum CREATE-ONLY (5821-5830: product bar3 top50
 * verify finalize). Unique gj_top50_verify_p_5828 surface only; no
 * multi-def. Pair with gj_top50_verify_threshold_5827 and
 * gj_product_top50_verify_score_5826. Distinct from
 * gj_title_ready_p_5728 (batch5728). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5828_marker[] = "libcgj-batch5828";

#define B5828_READY_MIN  70u
#define B5828_READY_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5828_verify_p(uint32_t uScore)
{
if (uScore < B5828_READY_MIN) {
return 0;
}
if (uScore > B5828_READY_MAX) {
return 0;
}
return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_top50_verify_p_5828 - soft product top50 verify lamp for score.
 *
 * score: aggregate product verify score (typically from 5826 / 5829).
 * Returns 1 when score is in [70, 100], else 0.
 * Does not launch titles. Does not call libc. No parent wires.
 */
int
gj_top50_verify_p_5828(uint32_t uScore)
{
(void)NULL;
return b5828_verify_p(uScore);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_top50_verify_p_5828(uint32_t uScore)
    __attribute__((alias("gj_top50_verify_p_5828")));
