/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7456: linkat has-known-flag compound stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_linkat_has_known_u_7456(uint32_t flags);
 *     - Return 1 if AT_SYMLINK_FOLLOW or AT_EMPTY_PATH is set, else 0.
 *   uint32_t __gj_linkat_has_known_u_7456  (alias)
 *   __libcgj_batch7456_marker = "libcgj-batch7456"
 *
 * Exclusive continuum CREATE-ONLY (7451-7460: linkat flags stubs —
 * symlink_follow, empty_path, both_mask, known_flags, known_flags_ok,
 * has_known, both_u, mask_u, any_u, continuum + batch_id_7460).
 * Unique gj_linkat_has_known_u_7456 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7456_marker[] = "libcgj-batch7456";

/* Soft known linkat bits: AT_SYMLINK_FOLLOW | AT_EMPTY_PATH. */
#define B7456_AT_SYMLINK_FOLLOW ((uint32_t)0x00000400u)
#define B7456_AT_EMPTY_PATH     ((uint32_t)0x00001000u)
#define B7456_LINKAT_KNOWN_MASK \
	(B7456_AT_SYMLINK_FOLLOW | B7456_AT_EMPTY_PATH)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7456_has_known(uint32_t u32Flags)
{
	return ((u32Flags & B7456_LINKAT_KNOWN_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_linkat_has_known_u_7456 - 1 if any known linkat flag bit is set.
 *
 * flags: raw linkat flags bitmask
 *
 * Returns 1 when AT_SYMLINK_FOLLOW and/or AT_EMPTY_PATH is present.
 * Self-contained; does not call sibling gj_linkat_* helpers.
 * No parent wires.
 */
uint32_t
gj_linkat_has_known_u_7456(uint32_t u32Flags)
{
	(void)NULL;
	return b7456_has_known(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_linkat_has_known_u_7456(uint32_t u32Flags)
    __attribute__((alias("gj_linkat_has_known_u_7456")));
