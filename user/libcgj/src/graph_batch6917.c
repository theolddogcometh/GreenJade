/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6917: renameat2 known-bits-only validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_renameat2_valid_u_6917(uint32_t flags);
 *     - Return 1 if no unknown flag bits are set, else 0.
 *   uint32_t __gj_renameat2_valid_u_6917  (alias)
 *   __libcgj_batch6917_marker = "libcgj-batch6917"
 *
 * Exclusive continuum CREATE-ONLY (6911-6920: renameat2 flags stubs —
 * noreplace, exchange, whiteout, any, mask, conflict, valid,
 * noreplace_ok, legal_combo, batch_id_6920). Unique
 * gj_renameat2_valid_u_6917 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6917_marker[] = "libcgj-batch6917";

#define B6917_RENAME_NOREPLACE ((uint32_t)0x00000001u)
#define B6917_RENAME_EXCHANGE  ((uint32_t)0x00000002u)
#define B6917_RENAME_WHITEOUT  ((uint32_t)0x00000004u)
#define B6917_KNOWN_MASK \
	(B6917_RENAME_NOREPLACE | B6917_RENAME_EXCHANGE | B6917_RENAME_WHITEOUT)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6917_renameat2_valid(uint32_t u32Flags)
{
	return ((u32Flags & ~B6917_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_renameat2_valid_u_6917 - 1 if flags use only known renameat2 bits.
 *
 * flags: raw renameat2 flags bitmask
 *
 * Returns 1 when every set bit is NOREPLACE, EXCHANGE, or WHITEOUT
 * (zero flags is valid). Does not check mutual-exclusion. No parent wires.
 */
uint32_t
gj_renameat2_valid_u_6917(uint32_t u32Flags)
{
	(void)NULL;
	return b6917_renameat2_valid(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_renameat2_valid_u_6917(uint32_t u32Flags)
    __attribute__((alias("gj_renameat2_valid_u_6917")));
