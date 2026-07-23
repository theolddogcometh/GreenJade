/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2292: dynamic export band soft-ok predicate.
 *
 * Surface (unique symbols):
 *   int gj_export_band_ok(uint32_t count);
 *     - Return 1 if count is in the closed product export band
 *       [1, 4096], else 0. Soft pure-data range for dynamic symbol
 *       tables. Distinct from gj_export_floor_ok (batch2194 floor-only)
 *       and gj_export_count_ok_hint (batch1997 upper 65536).
 *   int __gj_export_band_ok  (alias)
 *   __libcgj_batch2292_marker = "libcgj-batch2292"
 *
 * Milestone 2300 exclusive product helpers (2291-2300). Unique
 * gj_export_band_ok surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2292_marker[] = "libcgj-batch2292";

/* Inclusive product export band for milestone 2300 soft checks. */
#define B2292_BAND_LO  1u
#define B2292_BAND_HI  4096u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if count is in [B2292_BAND_LO, B2292_BAND_HI]. */
static int
b2292_band_ok(uint32_t uCount)
{
	if (uCount < B2292_BAND_LO) {
		return 0;
	}
	if (uCount > B2292_BAND_HI) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_band_ok - 1 iff export count is in the product band.
 *
 * count: reported dynamic export symbol count.
 * Band is [1, 4096] (non-empty, power-of-two headroom past typical
 * product SO tables). Does not parse ELF. Does not call libc.
 */
int
gj_export_band_ok(uint32_t uCount)
{
	(void)NULL;
	return b2292_band_ok(uCount);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_export_band_ok(uint32_t uCount)
    __attribute__((alias("gj_export_band_ok")));
