/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14552: pmm order1 soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pmm_order1_u_14552(void);
 *     - Returns 1 (soft PMM buddy order-1 tag for this continuum).
 *       Soft compile-time order identity; not a live page allocator probe.
 *   uint32_t __gj_pmm_order1_u_14552  (alias)
 *   __libcgj_batch14552_marker = "libcgj-batch14552"
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

const char __libcgj_batch14552_marker[] = "libcgj-batch14552";

/* PMM order-1 soft tag. */
#define B14552_ORDER1  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14552_order(void)
{
	return B14552_ORDER1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pmm_order1_u_14552 - soft continuum surface
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pmm_order1_u_14552(void)
{
	(void)NULL;
	return b14552_order();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pmm_order1_u_14552(void)
    __attribute__((alias("gj_pmm_order1_u_14552")));
