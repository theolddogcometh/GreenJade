/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6437: depot/DLC manifest entry-count soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_manifest_entry_count_ok_6437(uint32_t count);
 *     - Return 1 if count is in [1, 1048576] (soft file/chunk entry
 *       count band for a depot or DLC manifest), else 0.
 *   int __gj_manifest_entry_count_ok_6437  (alias)
 *   __libcgj_batch6437_marker = "libcgj-batch6437"
 *
 * Exclusive continuum CREATE-ONLY (6431-6440: DLC/depot manifest size
 * stubs — dlc_id_ok_6431, depot_id_ok_6432, manifest_size_ok_6433,
 * dlc_manifest_hdr_size_6434, depot_manifest_max_size_6435,
 * depot_chunk_size_ok_6436, manifest_entry_count_ok_6437,
 * dlc_size_pack_6438, depot_manifest_size_gate_6439, batch_id_6440).
 * Unique gj_manifest_entry_count_ok_6437 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6437_marker[] = "libcgj-batch6437";

/* Soft manifest entry count band (inclusive). */
#define B6437_ENTRY_MIN  1u
#define B6437_ENTRY_MAX  1048576u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6437_entry_ok(uint32_t uCount)
{
	if (uCount < B6437_ENTRY_MIN) {
		return 0;
	}
	if (uCount > B6437_ENTRY_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_manifest_entry_count_ok_6437 - soft manifest entry-count gate.
 *
 * count: number of file/chunk entries declared by a manifest.
 * Returns 1 when count is in [1, 1048576], else 0.
 * Does not parse entries. Does not call libc. No parent wires.
 */
int
gj_manifest_entry_count_ok_6437(uint32_t uCount)
{
	(void)NULL;
	return b6437_entry_ok(uCount);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_manifest_entry_count_ok_6437(uint32_t uCount)
    __attribute__((alias("gj_manifest_entry_count_ok_6437")));
