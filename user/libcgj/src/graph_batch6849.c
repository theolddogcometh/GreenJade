/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6849: close_range both-known-flags compound stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cr_both_u_6849(uint32_t flags);
 *     - Return 1 if both CLOSE_RANGE_UNSHARE and CLOSE_RANGE_CLOEXEC
 *       are set, else 0.
 *   uint32_t __gj_cr_both_u_6849  (alias)
 *   __libcgj_batch6849_marker = "libcgj-batch6849"
 *
 * Exclusive continuum CREATE-ONLY (6841-6850: close_range flags stubs —
 * unshare, cloexec, has_unshare, has_cloexec, known_mask, has_any,
 * flags_ok, plain, both, batch_id_6850). Unique gj_cr_both_u_6849
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6849_marker[] = "libcgj-batch6849";

#define B6849_CLOSE_RANGE_UNSHARE ((uint32_t)0x00000002u)
#define B6849_CLOSE_RANGE_CLOEXEC ((uint32_t)0x00000004u)
#define B6849_BOTH_MASK \
	(B6849_CLOSE_RANGE_UNSHARE | B6849_CLOSE_RANGE_CLOEXEC)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6849_both(uint32_t u32Flags)
{
	return ((u32Flags & B6849_BOTH_MASK) == B6849_BOTH_MASK) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cr_both_u_6849 - 1 if both documented close_range flags are set.
 *
 * flags: raw close_range(2) flags bitmask
 *
 * Returns 1 when UNSHARE and CLOEXEC are both present (extra bits ok).
 * Self-contained; does not call sibling gj_cr_* helpers. No parent wires.
 */
uint32_t
gj_cr_both_u_6849(uint32_t u32Flags)
{
	(void)NULL;
	return b6849_both(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cr_both_u_6849(uint32_t u32Flags)
    __attribute__((alias("gj_cr_both_u_6849")));
