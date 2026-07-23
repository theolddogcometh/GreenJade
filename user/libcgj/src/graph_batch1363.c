/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1363: heavy-hitter / top-k slot estimate.
 *
 * Surface (unique symbols):
 *   uint64_t gj_topk_estimate_slots(uint64_t k);
 *     — Counter / heap slots for tracking the top-k heavy hitters.
 *       Space-Saving / Misra-Gries style: slots = k (min 1 when k > 0).
 *       k == 0 → 0.
 *   uint64_t __gj_topk_estimate_slots  (alias)
 *   __libcgj_batch1363_marker = "libcgj-batch1363"
 *
 * Distinct from gj_topk_init/offer (batch497) and gj_topk_peek_min
 * (batch895). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1363_marker[] = "libcgj-batch1363";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_topk_estimate_slots — slots needed for k heavy hitters.
 *
 * u64K: requested top-k count. 0 → 0; otherwise return k (one slot per
 * tracked key). Callers that over-provision may scale this further.
 */
uint64_t
gj_topk_estimate_slots(uint64_t u64K)
{
	return u64K;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_topk_estimate_slots(uint64_t u64K)
    __attribute__((alias("gj_topk_estimate_slots")));
