/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7817: fnmatch flags validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fnmatch_flags_ok_u_7817(uint32_t flags);
 *     - Return 1 if flags uses only known FNM_* bits
 *       (0x1|0x2|0x10), else 0.
 *   uint32_t __gj_fnmatch_flags_ok_u_7817  (alias)
 *   __libcgj_batch7817_marker = "libcgj-batch7817"
 *
 * Exclusive continuum CREATE-ONLY (7811-7820: fnmatch flags stubs —
 * pathname_id, noescape_id, casefold_id, has_pathname, has_noescape,
 * has_casefold, flags_ok, flags_pack, flags_errorish, batch_id_7820).
 * Unique gj_fnmatch_flags_ok_u_7817 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7817_marker[] = "libcgj-batch7817";

/*
 * Known fnmatch flag bits for this continuum:
 *   FNM_PATHNAME (0x1) | FNM_NOESCAPE (0x2) | FNM_CASEFOLD (0x10) = 0x13
 */
#define B7817_FNM_KNOWN_MASK ((uint32_t)0x13u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7817_fnmatch_flags_ok(uint32_t u32Flags)
{
	return ((u32Flags & ~B7817_FNM_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fnmatch_flags_ok_u_7817 - 1 if flags has no unknown bits.
 *
 * flags: fnmatch(3) flags argument
 *
 * Returns 1 when only bits in {0x1, 0x2, 0x10} are set (including 0),
 * else 0. Soft catalog check; does not call fnmatch(3). No parent wires.
 */
uint32_t
gj_fnmatch_flags_ok_u_7817(uint32_t u32Flags)
{
	(void)NULL;
	return b7817_fnmatch_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fnmatch_flags_ok_u_7817(uint32_t u32Flags)
    __attribute__((alias("gj_fnmatch_flags_ok_u_7817")));
