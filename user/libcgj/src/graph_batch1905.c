/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1905: DPI scale of a pixel length (96-dpi base).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dpi_scale_u32(uint32_t px, uint32_t dpi);
 *     — Scale a length in device-independent pixels by dpi/96 using
 *       integer arithmetic: floor((px * dpi) / 96). Product is widened
 *       to uint64_t before divide. dpi==0 yields 0.
 *   uint32_t __gj_dpi_scale_u32  (alias)
 *   __libcgj_batch1905_marker = "libcgj-batch1905"
 *
 * Desktop window exclusive pure-data wave (1901–1910). Unique surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1905_marker[] = "libcgj-batch1905";

/* Windows/CSS logical DPI base (96 dpi = 100% scale). */
#define B1905_DPI_BASE  96u

/* ---- freestanding helpers ---------------------------------------------- */

/* floor((px * dpi) / 96); 0 if dpi is 0. Result truncated to u32. */
static uint32_t
b1905_scale(uint32_t u32Px, uint32_t u32Dpi)
{
	uint64_t u64Prod;

	if (u32Dpi == 0u) {
		return 0u;
	}
	u64Prod = (uint64_t)u32Px * (uint64_t)u32Dpi;
	return (uint32_t)(u64Prod / (uint64_t)B1905_DPI_BASE);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dpi_scale_u32 — scale px by dpi/96 (integer, floor).
 *
 * px:  length in 96-dpi reference pixels
 * dpi: target dots-per-inch (0 → 0)
 *
 * Returns floor(px * dpi / 96) as uint32_t.
 */
uint32_t
gj_dpi_scale_u32(uint32_t u32Px, uint32_t u32Dpi)
{
	(void)NULL;
	return b1905_scale(u32Px, u32Dpi);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dpi_scale_u32(uint32_t u32Px, uint32_t u32Dpi)
    __attribute__((alias("gj_dpi_scale_u32")));
