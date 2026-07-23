/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7457: linkat FOLLOW|EMPTY full-mask stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_linkat_both_u_7457(uint32_t flags);
 *     - Return 1 if both known linkat bits are set
 *       (AT_SYMLINK_FOLLOW|AT_EMPTY_PATH == 0x1400 subset of flags).
 *   uint32_t __gj_linkat_both_u_7457  (alias)
 *   __libcgj_batch7457_marker = "libcgj-batch7457"
 *
 * Exclusive continuum CREATE-ONLY (7451-7460: linkat flags stubs —
 * symlink_follow, empty_path, both_mask, known_flags, known_flags_ok,
 * has_known, both_u, mask_u, any_u, continuum + batch_id_7460).
 * Unique gj_linkat_both_u_7457 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7457_marker[] = "libcgj-batch7457";

/* Soft both-known: AT_SYMLINK_FOLLOW and AT_EMPTY_PATH together. */
#define B7457_AT_SYMLINK_FOLLOW ((uint32_t)0x00000400u)
#define B7457_AT_EMPTY_PATH     ((uint32_t)0x00001000u)
#define B7457_LINKAT_BOTH \
	(B7457_AT_SYMLINK_FOLLOW | B7457_AT_EMPTY_PATH)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7457_both(uint32_t u32Flags)
{
	return ((u32Flags & B7457_LINKAT_BOTH) == B7457_LINKAT_BOTH)
	    ? 1u
	    : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_linkat_both_u_7457 - 1 if full FOLLOW|EMPTY mask is set.
 *
 * flags: raw linkat flags bitmask
 *
 * Returns 1 when AT_SYMLINK_FOLLOW and AT_EMPTY_PATH are both present.
 * Self-contained; does not call sibling helpers. No parent wires.
 */
uint32_t
gj_linkat_both_u_7457(uint32_t u32Flags)
{
	(void)NULL;
	return b7457_both(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_linkat_both_u_7457(uint32_t u32Flags)
    __attribute__((alias("gj_linkat_both_u_7457")));
