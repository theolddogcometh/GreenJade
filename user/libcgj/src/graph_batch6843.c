/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6843: close_range CLOSE_RANGE_UNSHARE bit test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cr_has_unshare_u_6843(uint32_t flags);
 *     - Return 1 if CLOSE_RANGE_UNSHARE is set in flags, else 0.
 *   uint32_t __gj_cr_has_unshare_u_6843  (alias)
 *   __libcgj_batch6843_marker = "libcgj-batch6843"
 *
 * Exclusive continuum CREATE-ONLY (6841-6850: close_range flags stubs —
 * unshare, cloexec, has_unshare, has_cloexec, known_mask, has_any,
 * flags_ok, plain, both, batch_id_6850). Unique gj_cr_has_unshare_u_6843
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6843_marker[] = "libcgj-batch6843";

/* CLOSE_RANGE_UNSHARE: unshare file descriptor table first (1U << 1). */
#define B6843_CLOSE_RANGE_UNSHARE ((uint32_t)0x00000002u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6843_has_unshare(uint32_t u32Flags)
{
	return ((u32Flags & B6843_CLOSE_RANGE_UNSHARE) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cr_has_unshare_u_6843 - 1 if CLOSE_RANGE_UNSHARE is set.
 *
 * flags: raw close_range(2) flags bitmask
 *
 * Returns 1 when the unshare bit is set, else 0.
 * Pure mask test; no parent wires.
 */
uint32_t
gj_cr_has_unshare_u_6843(uint32_t u32Flags)
{
	(void)NULL;
	return b6843_has_unshare(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cr_has_unshare_u_6843(uint32_t u32Flags)
    __attribute__((alias("gj_cr_has_unshare_u_6843")));
