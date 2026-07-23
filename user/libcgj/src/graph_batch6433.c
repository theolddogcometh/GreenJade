/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6433: depot/DLC manifest byte-size soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_manifest_size_ok_6433(uint32_t size);
 *     - Return 1 if size is in [64, 268435456] (soft depot/DLC manifest
 *       payload band: header floor through 256 MiB), else 0.
 *   int __gj_manifest_size_ok_6433  (alias)
 *   __libcgj_batch6433_marker = "libcgj-batch6433"
 *
 * Exclusive continuum CREATE-ONLY (6431-6440: DLC/depot manifest size
 * stubs — dlc_id_ok_6431, depot_id_ok_6432, manifest_size_ok_6433,
 * dlc_manifest_hdr_size_6434, depot_manifest_max_size_6435,
 * depot_chunk_size_ok_6436, manifest_entry_count_ok_6437,
 * dlc_size_pack_6438, depot_manifest_size_gate_6439, batch_id_6440).
 * Unique gj_manifest_size_ok_6433 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6433_marker[] = "libcgj-batch6433";

/* Soft manifest payload size band (bytes, inclusive). */
#define B6433_MFST_SIZE_MIN  64u
#define B6433_MFST_SIZE_MAX  268435456u  /* 256 MiB */

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6433_size_ok(uint32_t uSize)
{
	if (uSize < B6433_MFST_SIZE_MIN) {
		return 0;
	}
	if (uSize > B6433_MFST_SIZE_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_manifest_size_ok_6433 - soft depot/DLC manifest byte-size gate.
 *
 * size: candidate manifest blob length in bytes.
 * Returns 1 when size is in [64, 268435456], else 0.
 * Does not parse a manifest. Does not call libc. No parent wires.
 */
int
gj_manifest_size_ok_6433(uint32_t uSize)
{
	(void)NULL;
	return b6433_size_ok(uSize);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_manifest_size_ok_6433(uint32_t uSize)
    __attribute__((alias("gj_manifest_size_ok_6433")));
