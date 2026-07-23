/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7818: fnmatch flags pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fnmatch_flags_pack_u_7818(uint32_t want_pathname,
 *                                         uint32_t want_noescape,
 *                                         uint32_t want_casefold);
 *     - Pack soft FNM_* wants into a flags mask (0x1|0x2|0x10).
 *   uint32_t __gj_fnmatch_flags_pack_u_7818  (alias)
 *   __libcgj_batch7818_marker = "libcgj-batch7818"
 *
 * Exclusive continuum CREATE-ONLY (7811-7820: fnmatch flags stubs —
 * pathname_id, noescape_id, casefold_id, has_pathname, has_noescape,
 * has_casefold, flags_ok, flags_pack, flags_errorish, batch_id_7820).
 * Unique gj_fnmatch_flags_pack_u_7818 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7818_marker[] = "libcgj-batch7818";

/* Soft pack bits matching POSIX/glibc FNM_* layout. */
#define B7818_FNM_PATHNAME ((uint32_t)0x1u)
#define B7818_FNM_NOESCAPE ((uint32_t)0x2u)
#define B7818_FNM_CASEFOLD ((uint32_t)0x10u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7818_fnmatch_flags_pack(uint32_t u32WantPathname,
                         uint32_t u32WantNoescape,
                         uint32_t u32WantCasefold)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantPathname != 0u)
		u32Pack |= B7818_FNM_PATHNAME;
	if (u32WantNoescape != 0u)
		u32Pack |= B7818_FNM_NOESCAPE;
	if (u32WantCasefold != 0u)
		u32Pack |= B7818_FNM_CASEFOLD;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fnmatch_flags_pack_u_7818 - soft-pack fnmatch flag wants.
 *
 * want_pathname: non-zero to set FNM_PATHNAME (0x1)
 * want_noescape: non-zero to set FNM_NOESCAPE (0x2)
 * want_casefold: non-zero to set FNM_CASEFOLD (0x10)
 *
 * Returns bitmask of requested FNM_* bits. Pure integer pack; does not
 * call fnmatch(3). No parent wires.
 */
uint32_t
gj_fnmatch_flags_pack_u_7818(uint32_t u32WantPathname,
                             uint32_t u32WantNoescape,
                             uint32_t u32WantCasefold)
{
	(void)NULL;
	return b7818_fnmatch_flags_pack(u32WantPathname, u32WantNoescape,
	                                u32WantCasefold);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fnmatch_flags_pack_u_7818(uint32_t u32WantPathname,
                                        uint32_t u32WantNoescape,
                                        uint32_t u32WantCasefold)
    __attribute__((alias("gj_fnmatch_flags_pack_u_7818")));
