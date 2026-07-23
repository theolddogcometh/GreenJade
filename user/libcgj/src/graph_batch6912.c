/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6912: renameat2 RENAME_EXCHANGE flag bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_renameat2_exchange_u_6912(uint32_t flags);
 *     - Return 1 if RENAME_EXCHANGE is set, else 0.
 *   uint32_t __gj_renameat2_exchange_u_6912  (alias)
 *   __libcgj_batch6912_marker = "libcgj-batch6912"
 *
 * Exclusive continuum CREATE-ONLY (6911-6920: renameat2 flags stubs —
 * noreplace, exchange, whiteout, any, mask, conflict, valid,
 * noreplace_ok, legal_combo, batch_id_6920). Unique
 * gj_renameat2_exchange_u_6912 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6912_marker[] = "libcgj-batch6912";

/* RENAME_EXCHANGE: atomically exchange oldpath and newpath (1 << 1). */
#define B6912_RENAME_EXCHANGE ((uint32_t)0x00000002u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6912_renameat2_exchange(uint32_t u32Flags)
{
	return ((u32Flags & B6912_RENAME_EXCHANGE) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_renameat2_exchange_u_6912 - 1 if RENAME_EXCHANGE is set.
 *
 * flags: raw renameat2 flags bitmask
 *
 * Returns 1 when the exchange bit is set, else 0.
 * Pure mask test; no parent wires.
 */
uint32_t
gj_renameat2_exchange_u_6912(uint32_t u32Flags)
{
	(void)NULL;
	return b6912_renameat2_exchange(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_renameat2_exchange_u_6912(uint32_t u32Flags)
    __attribute__((alias("gj_renameat2_exchange_u_6912")));
