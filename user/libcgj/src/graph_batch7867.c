/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7867: nl_langinfo item validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nl_item_ok_u_7867(uint32_t item);
 *     - Return 1 if item is a known soft nl_item (1|2|3), else 0.
 *   uint32_t __gj_nl_item_ok_u_7867  (alias)
 *   __libcgj_batch7867_marker = "libcgj-batch7867"
 *
 * Exclusive continuum CREATE-ONLY (7861-7870: nl_langinfo item stubs —
 * codeset_id, d_t_fmt_id, radixchar_id, is_codeset, is_d_t_fmt,
 * is_radixchar, item_ok, item_pack, item_errorish, batch_id_7870).
 * Unique gj_nl_item_ok_u_7867 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7867_marker[] = "libcgj-batch7867";

/*
 * Known soft nl_item catalog for this continuum:
 *   CODESET (1) | D_T_FMT (2) | RADIXCHAR (3)
 */
#define B7867_NL_CODESET   ((uint32_t)1u)
#define B7867_NL_D_T_FMT   ((uint32_t)2u)
#define B7867_NL_RADIXCHAR ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7867_nl_item_ok(uint32_t u32Item)
{
	if (u32Item == B7867_NL_CODESET)
		return 1u;
	if (u32Item == B7867_NL_D_T_FMT)
		return 1u;
	if (u32Item == B7867_NL_RADIXCHAR)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nl_item_ok_u_7867 - 1 if item is a known soft nl_item.
 *
 * item: soft nl_item catalog value
 *
 * Returns 1 when item is in {1, 2, 3}, else 0. Soft catalog check; does
 * not call nl_langinfo(3). No parent wires.
 */
uint32_t
gj_nl_item_ok_u_7867(uint32_t u32Item)
{
	(void)NULL;
	return b7867_nl_item_ok(u32Item);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nl_item_ok_u_7867(uint32_t u32Item)
    __attribute__((alias("gj_nl_item_ok_u_7867")));
