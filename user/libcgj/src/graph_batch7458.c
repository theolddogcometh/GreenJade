/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7458: linkat known-bits extract mask stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_linkat_mask_u_7458(uint32_t flags);
 *     - Return flags & known baseline (AT_SYMLINK_FOLLOW|AT_EMPTY_PATH).
 *   uint32_t __gj_linkat_mask_u_7458  (alias)
 *   __libcgj_batch7458_marker = "libcgj-batch7458"
 *
 * Exclusive continuum CREATE-ONLY (7451-7460: linkat flags stubs —
 * symlink_follow, empty_path, both_mask, known_flags, known_flags_ok,
 * has_known, both_u, mask_u, any_u, continuum + batch_id_7460).
 * Unique gj_linkat_mask_u_7458 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7458_marker[] = "libcgj-batch7458";

/* Soft known linkat flags: AT_SYMLINK_FOLLOW | AT_EMPTY_PATH. */
#define B7458_LINKAT_KNOWN_FLAGS ((uint32_t)0x00001400u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7458_mask(uint32_t u32Flags)
{
	return u32Flags & B7458_LINKAT_KNOWN_FLAGS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_linkat_mask_u_7458 - extract known linkat flag bits.
 *
 * flags: raw linkat flags bitmask
 *
 * Returns flags & 0x1400 (known bits only; unknown bits cleared).
 * Pure mask extract; no parent wires. Does not call linkat.
 */
uint32_t
gj_linkat_mask_u_7458(uint32_t u32Flags)
{
	(void)NULL;
	return b7458_mask(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_linkat_mask_u_7458(uint32_t u32Flags)
    __attribute__((alias("gj_linkat_mask_u_7458")));
