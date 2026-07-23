/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6236: HDR HLG nominal peak white (Lw) check.
 *
 * Surface (unique symbols):
 *   int gj_hdr_hlg_lw_ok_6236(uint32_t lw_nits);
 *     - Return 1 if lw_nits is a plausible HLG display peak white Lw in
 *       the soft policy window [100, 2000] cd/m^2 (common HLG consumer /
 *       production range around the 1000-nit reference), else 0.
 *   int __gj_hdr_hlg_lw_ok_6236  (alias)
 *   __libcgj_batch6236_marker = "libcgj-batch6236"
 *
 * Exclusive continuum CREATE-ONLY (6231-6240: HDR PQ/HLG transfer stubs).
 * Unique gj_hdr_hlg_lw_ok_6236 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6236_marker[] = "libcgj-batch6236";

/* Soft HLG Lw policy window (cd/m^2). */
#define B6236_LW_MIN_NITS  100u
#define B6236_LW_MAX_NITS  2000u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6236_lw_ok(uint32_t u32LwNits)
{
	if (u32LwNits < B6236_LW_MIN_NITS) {
		return 0;
	}
	if (u32LwNits > B6236_LW_MAX_NITS) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_hlg_lw_ok_6236 - 1 if Lw nits is in the soft HLG peak window.
 *
 * lw_nits: nominal peak luminance of the HLG display (Lw).
 * Screens Lw before system-gamma / OOTF stubs. Pure predicate; no EDID
 * probe. Does not call libc. No parent wires.
 */
int
gj_hdr_hlg_lw_ok_6236(uint32_t u32LwNits)
{
	(void)NULL;
	return b6236_lw_ok(u32LwNits);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hdr_hlg_lw_ok_6236(uint32_t u32LwNits)
    __attribute__((alias("gj_hdr_hlg_lw_ok_6236")));
