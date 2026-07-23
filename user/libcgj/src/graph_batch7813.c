/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7813: fnmatch FNM_CASEFOLD flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fnmatch_casefold_id_7813(void);
 *     - Return soft FNM_CASEFOLD constant (0x10).
 *   uint32_t __gj_fnmatch_casefold_id_7813  (alias)
 *   __libcgj_batch7813_marker = "libcgj-batch7813"
 *
 * Exclusive continuum CREATE-ONLY (7811-7820: fnmatch flags stubs —
 * pathname_id, noescape_id, casefold_id, has_pathname, has_noescape,
 * has_casefold, flags_ok, flags_pack, flags_errorish, batch_id_7820).
 * Unique gj_fnmatch_casefold_id_7813 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7813_marker[] = "libcgj-batch7813";

/* FNM_CASEFOLD: case-insensitive match (1 << 4). */
#define B7813_FNM_CASEFOLD ((uint32_t)0x10u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7813_fnmatch_casefold_id(void)
{
	return B7813_FNM_CASEFOLD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fnmatch_casefold_id_7813 - soft FNM_CASEFOLD flag constant.
 *
 * Always returns 0x10 (GNU/glibc FNM_CASEFOLD). Catalog id only; does
 * not call fnmatch(3). No parent wires.
 */
uint32_t
gj_fnmatch_casefold_id_7813(void)
{
	(void)NULL;
	return b7813_fnmatch_casefold_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fnmatch_casefold_id_7813(void)
    __attribute__((alias("gj_fnmatch_casefold_id_7813")));
