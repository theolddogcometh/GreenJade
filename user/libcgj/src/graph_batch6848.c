/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6848: close_range plain (zero-flags) stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cr_plain_u_6848(uint32_t flags);
 *     - Return 1 if flags is exactly 0 (plain close, no special
 *       close_range behavior), else 0.
 *   uint32_t __gj_cr_plain_u_6848  (alias)
 *   __libcgj_batch6848_marker = "libcgj-batch6848"
 *
 * Exclusive continuum CREATE-ONLY (6841-6850: close_range flags stubs —
 * unshare, cloexec, has_unshare, has_cloexec, known_mask, has_any,
 * flags_ok, plain, both, batch_id_6850). Unique gj_cr_plain_u_6848
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6848_marker[] = "libcgj-batch6848";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6848_plain(uint32_t u32Flags)
{
	return (u32Flags == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cr_plain_u_6848 - 1 if close_range flags are exactly zero.
 *
 * flags: raw close_range(2) flags bitmask
 *
 * Returns 1 for plain close (no UNSHARE, no CLOEXEC, no reserved bits).
 * Pure equality test; no parent wires.
 */
uint32_t
gj_cr_plain_u_6848(uint32_t u32Flags)
{
	(void)NULL;
	return b6848_plain(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cr_plain_u_6848(uint32_t u32Flags)
    __attribute__((alias("gj_cr_plain_u_6848")));
