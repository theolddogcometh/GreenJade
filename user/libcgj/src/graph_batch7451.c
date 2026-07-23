/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7451: linkat AT_SYMLINK_FOLLOW flag bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_linkat_symlink_follow_u_7451(uint32_t flags);
 *     - Return 1 if AT_SYMLINK_FOLLOW (0x400) is set, else 0.
 *   uint32_t __gj_linkat_symlink_follow_u_7451  (alias)
 *   __libcgj_batch7451_marker = "libcgj-batch7451"
 *
 * Exclusive continuum CREATE-ONLY (7451-7460: linkat flags stubs —
 * symlink_follow, empty_path, both_mask, known_flags, known_flags_ok,
 * has_known, both_u, mask_u, any_u, continuum + batch_id_7460).
 * Unique gj_linkat_symlink_follow_u_7451 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7451_marker[] = "libcgj-batch7451";

/* AT_SYMLINK_FOLLOW: follow symbolic links in oldpath (0x400). */
#define B7451_AT_SYMLINK_FOLLOW ((uint32_t)0x00000400u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7451_symlink_follow(uint32_t u32Flags)
{
	return ((u32Flags & B7451_AT_SYMLINK_FOLLOW) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_linkat_symlink_follow_u_7451 - 1 if AT_SYMLINK_FOLLOW is set.
 *
 * flags: raw linkat flags bitmask
 *
 * Returns 1 when the symlink-follow bit is set, else 0.
 * Pure mask test; no parent wires. Does not call linkat.
 */
uint32_t
gj_linkat_symlink_follow_u_7451(uint32_t u32Flags)
{
	(void)NULL;
	return b7451_symlink_follow(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_linkat_symlink_follow_u_7451(uint32_t u32Flags)
    __attribute__((alias("gj_linkat_symlink_follow_u_7451")));
