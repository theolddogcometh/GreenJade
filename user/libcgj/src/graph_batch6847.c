/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6847: close_range flags-ok reserved-bit check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cr_flags_ok_u_6847(uint32_t flags);
 *     - Return 1 if flags contains only documented bits (or zero),
 *       else 0 when any reserved bit is set.
 *   uint32_t __gj_cr_flags_ok_u_6847  (alias)
 *   __libcgj_batch6847_marker = "libcgj-batch6847"
 *
 * Exclusive continuum CREATE-ONLY (6841-6850: close_range flags stubs —
 * unshare, cloexec, has_unshare, has_cloexec, known_mask, has_any,
 * flags_ok, plain, both, batch_id_6850). Unique gj_cr_flags_ok_u_6847
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6847_marker[] = "libcgj-batch6847";

#define B6847_CLOSE_RANGE_UNSHARE ((uint32_t)0x00000002u)
#define B6847_CLOSE_RANGE_CLOEXEC ((uint32_t)0x00000004u)
#define B6847_KNOWN_MASK \
	(B6847_CLOSE_RANGE_UNSHARE | B6847_CLOSE_RANGE_CLOEXEC)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6847_flags_ok(uint32_t u32Flags)
{
	return ((u32Flags & ~B6847_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cr_flags_ok_u_6847 - 1 if flags has no reserved close_range bits.
 *
 * flags: raw close_range(2) flags bitmask
 *
 * Returns 1 when only UNSHARE/CLOEXEC (or none) are set. Soft validator
 * stub; does not call the kernel. No parent wires.
 */
uint32_t
gj_cr_flags_ok_u_6847(uint32_t u32Flags)
{
	(void)NULL;
	return b6847_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cr_flags_ok_u_6847(uint32_t u32Flags)
    __attribute__((alias("gj_cr_flags_ok_u_6847")));
