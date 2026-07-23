/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7819: fnmatch flags errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fnmatch_flags_errorish_u_7819(uint32_t flags);
 *     - Return 1 if flags has any unknown bits outside known FNM_*
 *       mask (0x13), else 0.
 *   uint32_t __gj_fnmatch_flags_errorish_u_7819  (alias)
 *   __libcgj_batch7819_marker = "libcgj-batch7819"
 *
 * Exclusive continuum CREATE-ONLY (7811-7820: fnmatch flags stubs —
 * pathname_id, noescape_id, casefold_id, has_pathname, has_noescape,
 * has_casefold, flags_ok, flags_pack, flags_errorish, batch_id_7820).
 * Unique gj_fnmatch_flags_errorish_u_7819 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7819_marker[] = "libcgj-batch7819";

/*
 * Known fnmatch flag bits for this continuum:
 *   FNM_PATHNAME | FNM_NOESCAPE | FNM_CASEFOLD = 0x13
 */
#define B7819_FNM_KNOWN_MASK ((uint32_t)0x13u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7819_fnmatch_flags_errorish(uint32_t u32Flags)
{
	return ((u32Flags & ~B7819_FNM_KNOWN_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fnmatch_flags_errorish_u_7819 - 1 if flags has unknown bits.
 *
 * flags: fnmatch(3) flags argument
 *
 * Returns 1 when any bit outside {0x1, 0x2, 0x10} is set, else 0.
 * Soft inverse of flags_ok; does not call fnmatch(3). No parent wires.
 */
uint32_t
gj_fnmatch_flags_errorish_u_7819(uint32_t u32Flags)
{
	(void)NULL;
	return b7819_fnmatch_flags_errorish(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fnmatch_flags_errorish_u_7819(uint32_t u32Flags)
    __attribute__((alias("gj_fnmatch_flags_errorish_u_7819")));
