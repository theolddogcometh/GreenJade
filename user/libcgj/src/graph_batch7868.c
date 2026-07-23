/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7868: nl_langinfo item want pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nl_item_pack_u_7868(uint32_t want_codeset,
 *                                   uint32_t want_d_t_fmt,
 *                                   uint32_t want_radixchar);
 *     - Pack soft item wants into a mask (0x1|0x2|0x4).
 *   uint32_t __gj_nl_item_pack_u_7868  (alias)
 *   __libcgj_batch7868_marker = "libcgj-batch7868"
 *
 * Exclusive continuum CREATE-ONLY (7861-7870: nl_langinfo item stubs —
 * codeset_id, d_t_fmt_id, radixchar_id, is_codeset, is_d_t_fmt,
 * is_radixchar, item_ok, item_pack, item_errorish, batch_id_7870).
 * Unique gj_nl_item_pack_u_7868 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7868_marker[] = "libcgj-batch7868";

/* Soft want bits for packed nl_item selection. */
#define B7868_WANT_CODESET   ((uint32_t)0x1u)
#define B7868_WANT_D_T_FMT   ((uint32_t)0x2u)
#define B7868_WANT_RADIXCHAR ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7868_nl_item_pack(uint32_t u32WantCodeset,
                   uint32_t u32WantDTFmt,
                   uint32_t u32WantRadix)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantCodeset != 0u)
		u32Pack |= B7868_WANT_CODESET;
	if (u32WantDTFmt != 0u)
		u32Pack |= B7868_WANT_D_T_FMT;
	if (u32WantRadix != 0u)
		u32Pack |= B7868_WANT_RADIXCHAR;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nl_item_pack_u_7868 - soft-pack nl_item wants.
 *
 * want_codeset:   non-zero to set CODESET bit (0x1)
 * want_d_t_fmt:   non-zero to set D_T_FMT bit (0x2)
 * want_radixchar: non-zero to set RADIXCHAR bit (0x4)
 *
 * Returns bitmask of requested soft items. Pure integer pack; does not
 * call nl_langinfo(3). No parent wires.
 */
uint32_t
gj_nl_item_pack_u_7868(uint32_t u32WantCodeset,
                       uint32_t u32WantDTFmt,
                       uint32_t u32WantRadix)
{
	(void)NULL;
	return b7868_nl_item_pack(u32WantCodeset, u32WantDTFmt,
	                          u32WantRadix);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nl_item_pack_u_7868(uint32_t u32WantCodeset,
                                  uint32_t u32WantDTFmt,
                                  uint32_t u32WantRadix)
    __attribute__((alias("gj_nl_item_pack_u_7868")));
