/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1867: Steam controller type range hint.
 *
 * Surface (unique symbols):
 *   int gj_controller_type_ok_hint(int type);
 *     — Return 1 if type is in the soft-valid Steam input/controller
 *       type range [0, 31] (covers known ESteamInputType-style codes
 *       plus spare room), else 0. Soft hint only.
 *   int __gj_controller_type_ok_hint  (alias)
 *   __libcgj_batch1867_marker = "libcgj-batch1867"
 *
 * Steam/desktop product exclusive pure-data wave (1861–1870).
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1867_marker[] = "libcgj-batch1867";

/* Soft range covering documented Steam input types with headroom. */
#define B1867_TYPE_MIN  0
#define B1867_TYPE_MAX  31

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1867_type_ok(int nType)
{
	if (nType < B1867_TYPE_MIN) {
		return 0;
	}
	if (nType > B1867_TYPE_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_controller_type_ok_hint — 1 iff type ∈ [0, 31].
 *
 * type: Steam controller / input type code (signed so out-of-range
 * negatives are rejected). Soft catalog-free hint only.
 */
int
gj_controller_type_ok_hint(int nType)
{
	(void)sizeof(NULL);
	return b1867_type_ok(nType);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_controller_type_ok_hint(int nType)
    __attribute__((alias("gj_controller_type_ok_hint")));
