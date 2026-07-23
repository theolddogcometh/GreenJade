/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6082: SD / microSD mmcblk index range check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_mmcblk_index_ok_6082(uint32_t index);
 *     - Returns 1 when index is in soft catalog range [0, 31], else 0.
 *   uint32_t __gj_sd_mmcblk_index_ok_6082  (alias)
 *   __libcgj_batch6082_marker = "libcgj-batch6082"
 *
 * Exclusive continuum CREATE-ONLY (6081-6090: SD card / microSD path
 * stubs). Unique gj_sd_mmcblk_index_ok_6082 surface only; no multi-def.
 * Distinct from part index (batch6083). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6082_marker[] = "libcgj-batch6082";

/* Soft max mmcblkN index (inclusive) for path construction stubs. */
#define B6082_MMCBLK_INDEX_MAX  31u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6082_index_ok(uint32_t uIndex)
{
	if (uIndex > B6082_MMCBLK_INDEX_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_mmcblk_index_ok_6082 - soft mmcblk device index range check.
 *
 * index: candidate N for mmcblkN path component.
 * Returns 1 when index <= 31, else 0. Path catalog bound only; does
 * not enumerate /sys/block. No parent wires.
 */
uint32_t
gj_sd_mmcblk_index_ok_6082(uint32_t uIndex)
{
	(void)NULL;
	return b6082_index_ok(uIndex);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sd_mmcblk_index_ok_6082(uint32_t uIndex)
    __attribute__((alias("gj_sd_mmcblk_index_ok_6082")));
