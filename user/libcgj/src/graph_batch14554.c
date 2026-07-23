/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14554: pmm order3 soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pmm_order3_u_14554(void);
 *     - Returns 3 (soft PMM buddy order-3 tag for this continuum).
 *       Soft compile-time order identity; not a live page allocator probe.
 *   uint32_t __gj_pmm_order3_u_14554  (alias)
 *   __libcgj_batch14554_marker = "libcgj-batch14554"
 *
 * Exclusive continuum CREATE-ONLY (14551-14560: pmm order soft
 * tags — order0..order7 identity; soft_ready→0; batch_id→14560). Unique surface
 * only; no multi-def. Distinct from gj_buddy_order_u and sibling pmm
 * order soft tags in prior waves. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14554_marker[] = "libcgj-batch14554";

/* PMM order-3 soft tag. */
#define B14554_ORDER3  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14554_order(void)
{
	return B14554_ORDER3;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pmm_order3_u_14554 - soft continuum surface
 *
 * Always returns 3u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pmm_order3_u_14554(void)
{
	(void)NULL;
	return b14554_order();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pmm_order3_u_14554(void)
    __attribute__((alias("gj_pmm_order3_u_14554")));
