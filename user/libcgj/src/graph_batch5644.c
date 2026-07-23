/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5644: installer format progress (0-100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_format_progress_5644(uint32_t done, uint32_t total);
 *     - Return percent complete (0..100) for format work units.
 *       total==0 → 0; done>=total → 100; else (done*100)/total.
 *   uint32_t gj_install_format_clamp_pct_5644(uint32_t pct);
 *     - Clamp a percent value into 0..100.
 *   uint32_t __gj_install_format_progress_5644  (alias)
 *   uint32_t __gj_install_format_clamp_pct_5644  (alias)
 *   __libcgj_batch5644_marker = "libcgj-batch5644"
 *
 * Exclusive continuum CREATE-ONLY (5641-5650: installer stages —
 * partition/format/copy/bootloader/config progress integers). Unique
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5644_marker[] = "libcgj-batch5644";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5644_clamp_pct(uint32_t u32Pct)
{
	if (u32Pct > 100u) {
		return 100u;
	}
	return u32Pct;
}

static uint32_t
b5644_progress(uint32_t u32Done, uint32_t u32Total)
{
	uint32_t u32Pct;

	if (u32Total == 0u) {
		return 0u;
	}
	if (u32Done >= u32Total) {
		return 100u;
	}
	if (u32Done > (UINT32_MAX / 100u)) {
		u32Pct = u32Done / (u32Total / 100u);
	} else {
		u32Pct = (u32Done * 100u) / u32Total;
	}
	return b5644_clamp_pct(u32Pct);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_format_progress_5644 - format stage percent complete.
 *
 * done:  completed format work units (blocks/steps)
 * total: total work units (0 → 0)
 *
 * Returns 0..100. Pure integer; no mkfs. No parent wires.
 */
uint32_t
gj_install_format_progress_5644(uint32_t u32Done, uint32_t u32Total)
{
	(void)NULL;
	return b5644_progress(u32Done, u32Total);
}

/*
 * gj_install_format_clamp_pct_5644 - clamp percent into 0..100.
 *
 * pct: raw percent (may be >100)
 *
 * Returns min(pct, 100).
 */
uint32_t
gj_install_format_clamp_pct_5644(uint32_t u32Pct)
{
	return b5644_clamp_pct(u32Pct);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_install_format_progress_5644(uint32_t u32Done, uint32_t u32Total)
    __attribute__((alias("gj_install_format_progress_5644")));

uint32_t __gj_install_format_clamp_pct_5644(uint32_t u32Pct)
    __attribute__((alias("gj_install_format_clamp_pct_5644")));
