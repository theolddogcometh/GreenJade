/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14458: pmm order7 soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pmm_order7_u_14458(void);
 *     - Returns 7 (soft PMM buddy order-7 tag for this continuum).
 *       Soft compile-time order identity; not a live page allocator probe.
 *   uint32_t __gj_pmm_order7_u_14458  (alias)
 *   __libcgj_batch14458_marker = "libcgj-batch14458"
 *
 * Exclusive continuum CREATE-ONLY (14451-14460: pmm order soft
 * tags — order0..order7 identity; soft_ready→0; batch_id→14460). Unique gj_pmm_order7_u_14458
 * surface only; no multi-def. Distinct from gj_buddy_order_u (batch3971) and sibling pmm
 * order soft tags in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14458_marker[] = "libcgj-batch14458";

/* PMM order-7 soft tag. */
#define B14458_ORDER7  7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14458_order(void)
{
	return B14458_ORDER7;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pmm_order7_u_14458 - PMM order7 soft tag
 *
 * Always returns 7u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pmm_order7_u_14458(void)
{
	(void)NULL;
	return b14458_order();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pmm_order7_u_14458(void)
    __attribute__((alias("gj_pmm_order7_u_14458")));
