/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5821: product bar3 Top50 rank verify gate.
 *
 * Surface (unique symbols):
 *   int gj_top50_rank_verify_ok_5821(uint32_t rank);
 *     - Return 1 if rank is in [1, 50] (Deck Top 50 matrix rank for
 *       product bar3 verify), else 0. Soft pure-data gate only.
 *   int __gj_top50_rank_verify_ok_5821  (alias)
 *   __libcgj_batch5821_marker = "libcgj-batch5821"
 *
 * Exclusive continuum CREATE-ONLY (5821-5830: product bar3 top50
 * verify finalize). Unique gj_top50_rank_verify_ok_5821 surface only;
 * no multi-def. Distinct from gj_title_rank_ok_5721 (batch5721). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5821_marker[] = "libcgj-batch5821";

/* Deck Top 50 matrix rank bounds (inclusive) for product verify. */
#define B5821_RANK_MIN  1u
#define B5821_RANK_MAX  50u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5821_rank_ok(uint32_t uRank)
{
if (uRank < B5821_RANK_MIN) {
return 0;
}
if (uRank > B5821_RANK_MAX) {
return 0;
}
return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_top50_rank_verify_ok_5821 - soft check Top50 rank is in verify range.
 *
 * rank: 1-based Deck Top 50 matrix rank for a product title.
 * Returns 1 when rank is in [1, 50], else 0.
 * Does not consult a catalog. Does not call libc. No parent wires.
 */
int
gj_top50_rank_verify_ok_5821(uint32_t uRank)
{
(void)NULL;
return b5821_rank_ok(uRank);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_top50_rank_verify_ok_5821(uint32_t uRank)
    __attribute__((alias("gj_top50_rank_verify_ok_5821")));
