/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7455: linkat known-flags subset check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_linkat_known_flags_ok_7455(uint32_t flags);
 *     - Return 1 if flags is a subset of the known baseline
 *       ((flags & ~0x1400) == 0).
 *   uint32_t __gj_linkat_known_flags_ok_7455  (alias)
 *   __libcgj_batch7455_marker = "libcgj-batch7455"
 *
 * Exclusive continuum CREATE-ONLY (7451-7460: linkat flags stubs —
 * symlink_follow, empty_path, both_mask, known_flags, known_flags_ok,
 * has_known, both_u, mask_u, any_u, continuum + batch_id_7460).
 * Unique gj_linkat_known_flags_ok_7455 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7455_marker[] = "libcgj-batch7455";

/* Soft known linkat flags: AT_SYMLINK_FOLLOW | AT_EMPTY_PATH. */
#define B7455_LINKAT_KNOWN_FLAGS ((uint32_t)0x00001400u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7455_known_flags_ok(uint32_t u32Flags)
{
	return ((u32Flags & ~B7455_LINKAT_KNOWN_FLAGS) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_linkat_known_flags_ok_7455 - flags is subset of known baseline.
 *
 * flags: soft linkat flags bitmask
 *
 * Returns 1 if no unknown bits present; else 0. Zero flags is ok.
 * Pure mask test; no parent wires. Does not call linkat.
 */
uint32_t
gj_linkat_known_flags_ok_7455(uint32_t u32Flags)
{
	(void)NULL;
	return b7455_known_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_linkat_known_flags_ok_7455(uint32_t u32Flags)
    __attribute__((alias("gj_linkat_known_flags_ok_7455")));
