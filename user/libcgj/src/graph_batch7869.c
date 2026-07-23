/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7869: nl_langinfo item errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nl_item_errorish_u_7869(uint32_t item);
 *     - Return 1 if item is outside known soft catalog {1,2,3}, else 0.
 *   uint32_t __gj_nl_item_errorish_u_7869  (alias)
 *   __libcgj_batch7869_marker = "libcgj-batch7869"
 *
 * Exclusive continuum CREATE-ONLY (7861-7870: nl_langinfo item stubs —
 * codeset_id, d_t_fmt_id, radixchar_id, is_codeset, is_d_t_fmt,
 * is_radixchar, item_ok, item_pack, item_errorish, batch_id_7870).
 * Unique gj_nl_item_errorish_u_7869 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7869_marker[] = "libcgj-batch7869";

/*
 * Known soft nl_item catalog for this continuum:
 *   CODESET (1) | D_T_FMT (2) | RADIXCHAR (3)
 */
#define B7869_NL_CODESET   ((uint32_t)1u)
#define B7869_NL_D_T_FMT   ((uint32_t)2u)
#define B7869_NL_RADIXCHAR ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7869_nl_item_errorish(uint32_t u32Item)
{
	if (u32Item == B7869_NL_CODESET)
		return 0u;
	if (u32Item == B7869_NL_D_T_FMT)
		return 0u;
	if (u32Item == B7869_NL_RADIXCHAR)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nl_item_errorish_u_7869 - 1 if item is unknown.
 *
 * item: soft nl_item catalog value
 *
 * Returns 1 when item is outside {1, 2, 3}, else 0. Soft inverse of
 * item_ok; does not call nl_langinfo(3). No parent wires.
 */
uint32_t
gj_nl_item_errorish_u_7869(uint32_t u32Item)
{
	(void)NULL;
	return b7869_nl_item_errorish(u32Item);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nl_item_errorish_u_7869(uint32_t u32Item)
    __attribute__((alias("gj_nl_item_errorish_u_7869")));
