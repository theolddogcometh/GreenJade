/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7459: linkat any-known-flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_linkat_any_u_7459(uint32_t flags);
 *     - Return 1 if any known linkat flag bit is set
 *       ((flags & 0x1400) != 0), else 0.
 *   uint32_t __gj_linkat_any_u_7459  (alias)
 *   __libcgj_batch7459_marker = "libcgj-batch7459"
 *
 * Exclusive continuum CREATE-ONLY (7451-7460: linkat flags stubs —
 * symlink_follow, empty_path, both_mask, known_flags, known_flags_ok,
 * has_known, both_u, mask_u, any_u, continuum + batch_id_7460).
 * Unique gj_linkat_any_u_7459 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7459_marker[] = "libcgj-batch7459";

/* Soft known linkat flags: AT_SYMLINK_FOLLOW | AT_EMPTY_PATH. */
#define B7459_LINKAT_KNOWN_FLAGS ((uint32_t)0x00001400u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7459_any(uint32_t u32Flags)
{
	return ((u32Flags & B7459_LINKAT_KNOWN_FLAGS) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_linkat_any_u_7459 - 1 if any known linkat flag is set.
 *
 * flags: raw linkat flags bitmask
 *
 * Returns 1 when at least one of AT_SYMLINK_FOLLOW, AT_EMPTY_PATH is set.
 * Pure mask test; no parent wires. Does not call linkat.
 */
uint32_t
gj_linkat_any_u_7459(uint32_t u32Flags)
{
	(void)NULL;
	return b7459_any(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_linkat_any_u_7459(uint32_t u32Flags)
    __attribute__((alias("gj_linkat_any_u_7459")));
