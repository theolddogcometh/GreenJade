/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5642: installer partition progress (0-100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_partition_progress_5642(uint32_t done, uint32_t total);
 *     - Return percent complete (0..100) for partition work units.
 *       total==0 → 0; done>=total → 100; else (done*100)/total.
 *   uint32_t gj_install_partition_clamp_pct_5642(uint32_t pct);
 *     - Clamp a percent value into 0..100.
 *   uint32_t __gj_install_partition_progress_5642  (alias)
 *   uint32_t __gj_install_partition_clamp_pct_5642  (alias)
 *   __libcgj_batch5642_marker = "libcgj-batch5642"
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

const char __libcgj_batch5642_marker[] = "libcgj-batch5642";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5642_clamp_pct(uint32_t u32Pct)
{
	if (u32Pct > 100u) {
		return 100u;
	}
	return u32Pct;
}

static uint32_t
b5642_progress(uint32_t u32Done, uint32_t u32Total)
{
	uint32_t u32Pct;

	if (u32Total == 0u) {
		return 0u;
	}
	if (u32Done >= u32Total) {
		return 100u;
	}
	/* (done * 100) / total; both < 2^32 so product may wrap — guard. */
	if (u32Done > (UINT32_MAX / 100u)) {
		u32Pct = u32Done / (u32Total / 100u);
	} else {
		u32Pct = (u32Done * 100u) / u32Total;
	}
	return b5642_clamp_pct(u32Pct);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_partition_progress_5642 - partition stage percent complete.
 *
 * done:  completed work units (sectors/steps)
 * total: total work units (0 → 0)
 *
 * Returns 0..100. Pure integer; no block devices. No parent wires.
 */
uint32_t
gj_install_partition_progress_5642(uint32_t u32Done, uint32_t u32Total)
{
	(void)NULL;
	return b5642_progress(u32Done, u32Total);
}

/*
 * gj_install_partition_clamp_pct_5642 - clamp percent into 0..100.
 *
 * pct: raw percent (may be >100)
 *
 * Returns min(pct, 100).
 */
uint32_t
gj_install_partition_clamp_pct_5642(uint32_t u32Pct)
{
	return b5642_clamp_pct(u32Pct);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_install_partition_progress_5642(uint32_t u32Done, uint32_t u32Total)
    __attribute__((alias("gj_install_partition_progress_5642")));

uint32_t __gj_install_partition_clamp_pct_5642(uint32_t u32Pct)
    __attribute__((alias("gj_install_partition_clamp_pct_5642")));
