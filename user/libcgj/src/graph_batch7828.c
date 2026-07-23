/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7828: wordexp flags pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wordexp_flags_pack_u_7828(uint32_t want_dooffs,
 *                                         uint32_t want_append,
 *                                         uint32_t want_nocmd);
 *     - Pack soft WRDE_* wants into a flags mask (0x1|0x2|0x4).
 *   uint32_t __gj_wordexp_flags_pack_u_7828  (alias)
 *   __libcgj_batch7828_marker = "libcgj-batch7828"
 *
 * Exclusive continuum CREATE-ONLY (7821-7830: wordexp flags stubs —
 * dooffs_id, append_id, nocmd_id, has_dooffs, has_append, has_nocmd,
 * flags_ok, flags_pack, flags_errorish, batch_id_7830).
 * Unique gj_wordexp_flags_pack_u_7828 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7828_marker[] = "libcgj-batch7828";

/* Soft pack bits matching POSIX/glibc WRDE_* layout. */
#define B7828_WRDE_DOOFFS ((uint32_t)0x1u)
#define B7828_WRDE_APPEND ((uint32_t)0x2u)
#define B7828_WRDE_NOCMD  ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7828_wordexp_flags_pack(uint32_t u32WantDooffs,
                         uint32_t u32WantAppend,
                         uint32_t u32WantNocmd)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantDooffs != 0u)
		u32Pack |= B7828_WRDE_DOOFFS;
	if (u32WantAppend != 0u)
		u32Pack |= B7828_WRDE_APPEND;
	if (u32WantNocmd != 0u)
		u32Pack |= B7828_WRDE_NOCMD;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wordexp_flags_pack_u_7828 - soft-pack wordexp flag wants.
 *
 * want_dooffs: non-zero to set WRDE_DOOFFS (0x1)
 * want_append: non-zero to set WRDE_APPEND (0x2)
 * want_nocmd:  non-zero to set WRDE_NOCMD (0x4)
 *
 * Returns bitmask of requested WRDE_* bits. Pure integer pack; does not
 * call wordexp(3). No parent wires.
 */
uint32_t
gj_wordexp_flags_pack_u_7828(uint32_t u32WantDooffs,
                             uint32_t u32WantAppend,
                             uint32_t u32WantNocmd)
{
	(void)NULL;
	return b7828_wordexp_flags_pack(u32WantDooffs, u32WantAppend,
	                                u32WantNocmd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wordexp_flags_pack_u_7828(uint32_t u32WantDooffs,
                                        uint32_t u32WantAppend,
                                        uint32_t u32WantNocmd)
    __attribute__((alias("gj_wordexp_flags_pack_u_7828")));
