/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6919: renameat2 legal flag combination stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_renameat2_legal_combo_u_6919(uint32_t flags);
 *     - Return 1 if flags have no unknown bits and no mutual-exclusion
 *       violations (NOREPLACE+EXCHANGE, WHITEOUT+EXCHANGE), else 0.
 *   uint32_t __gj_renameat2_legal_combo_u_6919  (alias)
 *   __libcgj_batch6919_marker = "libcgj-batch6919"
 *
 * Exclusive continuum CREATE-ONLY (6911-6920: renameat2 flags stubs —
 * noreplace, exchange, whiteout, any, mask, conflict, valid,
 * noreplace_ok, legal_combo, batch_id_6920). Unique
 * gj_renameat2_legal_combo_u_6919 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6919_marker[] = "libcgj-batch6919";

#define B6919_RENAME_NOREPLACE ((uint32_t)0x00000001u)
#define B6919_RENAME_EXCHANGE  ((uint32_t)0x00000002u)
#define B6919_RENAME_WHITEOUT  ((uint32_t)0x00000004u)
#define B6919_KNOWN_MASK \
	(B6919_RENAME_NOREPLACE | B6919_RENAME_EXCHANGE | B6919_RENAME_WHITEOUT)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6919_renameat2_legal_combo(uint32_t u32Flags)
{
	if ((u32Flags & ~B6919_KNOWN_MASK) != 0u)
		return 0u;
	if ((u32Flags & B6919_RENAME_EXCHANGE) != 0u) {
		if ((u32Flags & B6919_RENAME_NOREPLACE) != 0u)
			return 0u;
		if ((u32Flags & B6919_RENAME_WHITEOUT) != 0u)
			return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_renameat2_legal_combo_u_6919 - 1 if renameat2 flags are a legal combo.
 *
 * flags: raw renameat2 flags bitmask
 *
 * Rejects unknown bits and the Linux-forbidden pairings EXCHANGE with
 * NOREPLACE or WHITEOUT. Zero flags is legal. Self-contained; no parent
 * wires.
 */
uint32_t
gj_renameat2_legal_combo_u_6919(uint32_t u32Flags)
{
	(void)NULL;
	return b6919_renameat2_legal_combo(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_renameat2_legal_combo_u_6919(uint32_t u32Flags)
    __attribute__((alias("gj_renameat2_legal_combo_u_6919")));
