/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14553: pmm order2 soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pmm_order2_u_14553(void);
 *     - Returns 2 (soft PMM buddy order-2 tag for this continuum).
 *       Soft compile-time order identity; not a live page allocator probe.
 *   uint32_t __gj_pmm_order2_u_14553  (alias)
 *   __libcgj_batch14553_marker = "libcgj-batch14553"
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

const char __libcgj_batch14553_marker[] = "libcgj-batch14553";

/* PMM order-2 soft tag. */
#define B14553_ORDER2  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14553_order(void)
{
	return B14553_ORDER2;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pmm_order2_u_14553 - soft continuum surface
 *
 * Always returns 2u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pmm_order2_u_14553(void)
{
	(void)NULL;
	return b14553_order();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pmm_order2_u_14553(void)
    __attribute__((alias("gj_pmm_order2_u_14553")));
