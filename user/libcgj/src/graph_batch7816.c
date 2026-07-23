/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7816: fnmatch FNM_CASEFOLD presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fnmatch_has_casefold_u_7816(uint32_t flags);
 *     - Return 1 if flags has FNM_CASEFOLD (0x10) set, else 0.
 *   uint32_t __gj_fnmatch_has_casefold_u_7816  (alias)
 *   __libcgj_batch7816_marker = "libcgj-batch7816"
 *
 * Exclusive continuum CREATE-ONLY (7811-7820: fnmatch flags stubs —
 * pathname_id, noescape_id, casefold_id, has_pathname, has_noescape,
 * has_casefold, flags_ok, flags_pack, flags_errorish, batch_id_7820).
 * Unique gj_fnmatch_has_casefold_u_7816 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7816_marker[] = "libcgj-batch7816";

/* FNM_CASEFOLD bit (0x10). */
#define B7816_FNM_CASEFOLD ((uint32_t)0x10u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7816_fnmatch_has_casefold(uint32_t u32Flags)
{
	return ((u32Flags & B7816_FNM_CASEFOLD) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fnmatch_has_casefold_u_7816 - 1 if flags includes FNM_CASEFOLD.
 *
 * flags: fnmatch(3) flags argument
 *
 * Returns 1 when bit 0x10 is set, else 0. Soft bit test only; does
 * not call fnmatch(3). No parent wires.
 */
uint32_t
gj_fnmatch_has_casefold_u_7816(uint32_t u32Flags)
{
	(void)NULL;
	return b7816_fnmatch_has_casefold(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fnmatch_has_casefold_u_7816(uint32_t u32Flags)
    __attribute__((alias("gj_fnmatch_has_casefold_u_7816")));
