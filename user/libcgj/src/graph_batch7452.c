/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7452: linkat AT_EMPTY_PATH flag bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_linkat_empty_path_u_7452(uint32_t flags);
 *     - Return 1 if AT_EMPTY_PATH (0x1000) is set, else 0.
 *   uint32_t __gj_linkat_empty_path_u_7452  (alias)
 *   __libcgj_batch7452_marker = "libcgj-batch7452"
 *
 * Exclusive continuum CREATE-ONLY (7451-7460: linkat flags stubs —
 * symlink_follow, empty_path, both_mask, known_flags, known_flags_ok,
 * has_known, both_u, mask_u, any_u, continuum + batch_id_7460).
 * Unique gj_linkat_empty_path_u_7452 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7452_marker[] = "libcgj-batch7452";

/* AT_EMPTY_PATH: allow empty relative oldpath (0x1000). */
#define B7452_AT_EMPTY_PATH ((uint32_t)0x00001000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7452_empty_path(uint32_t u32Flags)
{
	return ((u32Flags & B7452_AT_EMPTY_PATH) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_linkat_empty_path_u_7452 - 1 if AT_EMPTY_PATH is set.
 *
 * flags: raw linkat flags bitmask
 *
 * Returns 1 when the empty-path bit is set, else 0.
 * Pure mask test; no parent wires. Does not call linkat.
 */
uint32_t
gj_linkat_empty_path_u_7452(uint32_t u32Flags)
{
	(void)NULL;
	return b7452_empty_path(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_linkat_empty_path_u_7452(uint32_t u32Flags)
    __attribute__((alias("gj_linkat_empty_path_u_7452")));
