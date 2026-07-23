/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6914: renameat2 any-known-flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_renameat2_any_u_6914(uint32_t flags);
 *     - Return 1 if any known renameat2 flag bit is set, else 0.
 *   uint32_t __gj_renameat2_any_u_6914  (alias)
 *   __libcgj_batch6914_marker = "libcgj-batch6914"
 *
 * Exclusive continuum CREATE-ONLY (6911-6920: renameat2 flags stubs —
 * noreplace, exchange, whiteout, any, mask, conflict, valid,
 * noreplace_ok, legal_combo, batch_id_6920). Unique
 * gj_renameat2_any_u_6914 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6914_marker[] = "libcgj-batch6914";

#define B6914_RENAME_NOREPLACE ((uint32_t)0x00000001u)
#define B6914_RENAME_EXCHANGE  ((uint32_t)0x00000002u)
#define B6914_RENAME_WHITEOUT  ((uint32_t)0x00000004u)
#define B6914_KNOWN_MASK \
	(B6914_RENAME_NOREPLACE | B6914_RENAME_EXCHANGE | B6914_RENAME_WHITEOUT)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6914_renameat2_any(uint32_t u32Flags)
{
	return ((u32Flags & B6914_KNOWN_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_renameat2_any_u_6914 - 1 if any known renameat2 flag is set.
 *
 * flags: raw renameat2 flags bitmask
 *
 * Returns 1 when NOREPLACE, EXCHANGE, or WHITEOUT is present, else 0.
 * Self-contained; no sibling calls. No parent wires.
 */
uint32_t
gj_renameat2_any_u_6914(uint32_t u32Flags)
{
	(void)NULL;
	return b6914_renameat2_any(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_renameat2_any_u_6914(uint32_t u32Flags)
    __attribute__((alias("gj_renameat2_any_u_6914")));
