/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6083: SD / microSD partition index range check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_part_index_ok_6083(uint32_t part);
 *     - Returns 1 when part is in soft partition range [1, 15], else 0.
 *   uint32_t __gj_sd_part_index_ok_6083  (alias)
 *   __libcgj_batch6083_marker = "libcgj-batch6083"
 *
 * Exclusive continuum CREATE-ONLY (6081-6090: SD card / microSD path
 * stubs). Unique gj_sd_part_index_ok_6083 surface only; no multi-def.
 * Distinct from mmcblk index (batch6082). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6083_marker[] = "libcgj-batch6083";

/* Soft partition suffix range for mmcblkNpM path stubs (1..15). */
#define B6083_PART_INDEX_MIN  1u
#define B6083_PART_INDEX_MAX  15u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6083_part_ok(uint32_t uPart)
{
	if (uPart < B6083_PART_INDEX_MIN) {
		return 0u;
	}
	if (uPart > B6083_PART_INDEX_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_part_index_ok_6083 - soft SD partition index range check.
 *
 * part: candidate M for mmcblkNpM partition path suffix.
 * Returns 1 when 1 <= part <= 15, else 0. Catalog bound only; does
 * not read partition tables. No parent wires.
 */
uint32_t
gj_sd_part_index_ok_6083(uint32_t uPart)
{
	(void)NULL;
	return b6083_part_ok(uPart);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sd_part_index_ok_6083(uint32_t uPart)
    __attribute__((alias("gj_sd_part_index_ok_6083")));
