/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7811: fnmatch FNM_PATHNAME flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fnmatch_pathname_id_7811(void);
 *     - Return soft FNM_PATHNAME constant (0x1).
 *   uint32_t __gj_fnmatch_pathname_id_7811  (alias)
 *   __libcgj_batch7811_marker = "libcgj-batch7811"
 *
 * Exclusive continuum CREATE-ONLY (7811-7820: fnmatch flags stubs —
 * pathname_id, noescape_id, casefold_id, has_pathname, has_noescape,
 * has_casefold, flags_ok, flags_pack, flags_errorish, batch_id_7820).
 * Unique gj_fnmatch_pathname_id_7811 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7811_marker[] = "libcgj-batch7811";

/* FNM_PATHNAME: slash must match slash (1 << 0). */
#define B7811_FNM_PATHNAME ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7811_fnmatch_pathname_id(void)
{
	return B7811_FNM_PATHNAME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fnmatch_pathname_id_7811 - soft FNM_PATHNAME flag constant.
 *
 * Always returns 0x1 (POSIX/glibc FNM_PATHNAME). Catalog id only; does
 * not call fnmatch(3). No parent wires.
 */
uint32_t
gj_fnmatch_pathname_id_7811(void)
{
	(void)NULL;
	return b7811_fnmatch_pathname_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fnmatch_pathname_id_7811(void)
    __attribute__((alias("gj_fnmatch_pathname_id_7811")));
