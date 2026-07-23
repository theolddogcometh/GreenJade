/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14457: pmm order6 soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pmm_order6_u_14457(void);
 *     - Returns 6 (soft PMM buddy order-6 tag for this continuum).
 *       Soft compile-time order identity; not a live page allocator probe.
 *   uint32_t __gj_pmm_order6_u_14457  (alias)
 *   __libcgj_batch14457_marker = "libcgj-batch14457"
 *
 * Exclusive continuum CREATE-ONLY (14451-14460: pmm order soft
 * tags — order0..order7 identity; soft_ready→0; batch_id→14460). Unique gj_pmm_order6_u_14457
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

const char __libcgj_batch14457_marker[] = "libcgj-batch14457";

/* PMM order-6 soft tag. */
#define B14457_ORDER6  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14457_order(void)
{
	return B14457_ORDER6;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pmm_order6_u_14457 - PMM order6 soft tag
 *
 * Always returns 6u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pmm_order6_u_14457(void)
{
	(void)NULL;
	return b14457_order();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pmm_order6_u_14457(void)
    __attribute__((alias("gj_pmm_order6_u_14457")));
