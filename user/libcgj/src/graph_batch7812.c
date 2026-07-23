/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7812: fnmatch FNM_NOESCAPE flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fnmatch_noescape_id_7812(void);
 *     - Return soft FNM_NOESCAPE constant (0x2).
 *   uint32_t __gj_fnmatch_noescape_id_7812  (alias)
 *   __libcgj_batch7812_marker = "libcgj-batch7812"
 *
 * Exclusive continuum CREATE-ONLY (7811-7820: fnmatch flags stubs —
 * pathname_id, noescape_id, casefold_id, has_pathname, has_noescape,
 * has_casefold, flags_ok, flags_pack, flags_errorish, batch_id_7820).
 * Unique gj_fnmatch_noescape_id_7812 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7812_marker[] = "libcgj-batch7812";

/* FNM_NOESCAPE: disable backslash escapes (1 << 1). */
#define B7812_FNM_NOESCAPE ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7812_fnmatch_noescape_id(void)
{
	return B7812_FNM_NOESCAPE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fnmatch_noescape_id_7812 - soft FNM_NOESCAPE flag constant.
 *
 * Always returns 0x2 (POSIX/glibc FNM_NOESCAPE). Catalog id only; does
 * not call fnmatch(3). No parent wires.
 */
uint32_t
gj_fnmatch_noescape_id_7812(void)
{
	(void)NULL;
	return b7812_fnmatch_noescape_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fnmatch_noescape_id_7812(void)
    __attribute__((alias("gj_fnmatch_noescape_id_7812")));
