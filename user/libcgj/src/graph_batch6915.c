/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6915: renameat2 known-flags mask extract stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_renameat2_mask_u_6915(uint32_t flags);
 *     - Return flags restricted to the known renameat2 bits.
 *   uint32_t __gj_renameat2_mask_u_6915  (alias)
 *   __libcgj_batch6915_marker = "libcgj-batch6915"
 *
 * Exclusive continuum CREATE-ONLY (6911-6920: renameat2 flags stubs —
 * noreplace, exchange, whiteout, any, mask, conflict, valid,
 * noreplace_ok, legal_combo, batch_id_6920). Unique
 * gj_renameat2_mask_u_6915 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6915_marker[] = "libcgj-batch6915";

#define B6915_RENAME_NOREPLACE ((uint32_t)0x00000001u)
#define B6915_RENAME_EXCHANGE  ((uint32_t)0x00000002u)
#define B6915_RENAME_WHITEOUT  ((uint32_t)0x00000004u)
#define B6915_KNOWN_MASK \
	(B6915_RENAME_NOREPLACE | B6915_RENAME_EXCHANGE | B6915_RENAME_WHITEOUT)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6915_renameat2_mask(uint32_t u32Flags)
{
	return u32Flags & B6915_KNOWN_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_renameat2_mask_u_6915 - strip unknown bits from renameat2 flags.
 *
 * flags: raw renameat2 flags bitmask
 *
 * Returns flags & (NOREPLACE|EXCHANGE|WHITEOUT). Pure mask; no parent
 * wires.
 */
uint32_t
gj_renameat2_mask_u_6915(uint32_t u32Flags)
{
	(void)NULL;
	return b6915_renameat2_mask(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_renameat2_mask_u_6915(uint32_t u32Flags)
    __attribute__((alias("gj_renameat2_mask_u_6915")));
