/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6918: renameat2 NOREPLACE-without-EXCHANGE stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_renameat2_noreplace_ok_u_6918(uint32_t flags);
 *     - Return 1 if NOREPLACE is set and EXCHANGE is clear, else 0.
 *   uint32_t __gj_renameat2_noreplace_ok_u_6918  (alias)
 *   __libcgj_batch6918_marker = "libcgj-batch6918"
 *
 * Exclusive continuum CREATE-ONLY (6911-6920: renameat2 flags stubs —
 * noreplace, exchange, whiteout, any, mask, conflict, valid,
 * noreplace_ok, legal_combo, batch_id_6920). Unique
 * gj_renameat2_noreplace_ok_u_6918 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6918_marker[] = "libcgj-batch6918";

#define B6918_RENAME_NOREPLACE ((uint32_t)0x00000001u)
#define B6918_RENAME_EXCHANGE  ((uint32_t)0x00000002u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6918_renameat2_noreplace_ok(uint32_t u32Flags)
{
	if ((u32Flags & B6918_RENAME_NOREPLACE) == 0u)
		return 0u;
	if ((u32Flags & B6918_RENAME_EXCHANGE) != 0u)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_renameat2_noreplace_ok_u_6918 - 1 if NOREPLACE set without EXCHANGE.
 *
 * flags: raw renameat2 flags bitmask
 *
 * Returns 1 for a non-conflicting NOREPLACE request (WHITEOUT may also
 * be present). Pure mask logic; no parent wires.
 */
uint32_t
gj_renameat2_noreplace_ok_u_6918(uint32_t u32Flags)
{
	(void)NULL;
	return b6918_renameat2_noreplace_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_renameat2_noreplace_ok_u_6918(uint32_t u32Flags)
    __attribute__((alias("gj_renameat2_noreplace_ok_u_6918")));
