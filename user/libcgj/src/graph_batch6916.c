/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6916: renameat2 NOREPLACE+EXCHANGE conflict stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_renameat2_conflict_u_6916(uint32_t flags);
 *     - Return 1 if NOREPLACE and EXCHANGE are both set (illegal), else 0.
 *   uint32_t __gj_renameat2_conflict_u_6916  (alias)
 *   __libcgj_batch6916_marker = "libcgj-batch6916"
 *
 * Exclusive continuum CREATE-ONLY (6911-6920: renameat2 flags stubs —
 * noreplace, exchange, whiteout, any, mask, conflict, valid,
 * noreplace_ok, legal_combo, batch_id_6920). Unique
 * gj_renameat2_conflict_u_6916 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6916_marker[] = "libcgj-batch6916";

#define B6916_RENAME_NOREPLACE ((uint32_t)0x00000001u)
#define B6916_RENAME_EXCHANGE  ((uint32_t)0x00000002u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6916_renameat2_conflict(uint32_t u32Flags)
{
	uint32_t u32Both = B6916_RENAME_NOREPLACE | B6916_RENAME_EXCHANGE;

	return ((u32Flags & u32Both) == u32Both) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_renameat2_conflict_u_6916 - 1 if NOREPLACE and EXCHANGE both set.
 *
 * flags: raw renameat2 flags bitmask
 *
 * Linux forbids combining those two bits. Returns 1 on that conflict,
 * else 0. Pure mask test; no parent wires.
 */
uint32_t
gj_renameat2_conflict_u_6916(uint32_t u32Flags)
{
	(void)NULL;
	return b6916_renameat2_conflict(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_renameat2_conflict_u_6916(uint32_t u32Flags)
    __attribute__((alias("gj_renameat2_conflict_u_6916")));
