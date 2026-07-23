/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6435: depot manifest max size constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_depot_manifest_max_size_6435(void);
 *     - Returns the soft maximum depot/DLC manifest payload size in
 *       bytes (268435456 == 256 MiB). Integer-only ceiling stub.
 *   uint32_t __gj_depot_manifest_max_size_6435  (alias)
 *   __libcgj_batch6435_marker = "libcgj-batch6435"
 *
 * Exclusive continuum CREATE-ONLY (6431-6440: DLC/depot manifest size
 * stubs — dlc_id_ok_6431, depot_id_ok_6432, manifest_size_ok_6433,
 * dlc_manifest_hdr_size_6434, depot_manifest_max_size_6435,
 * depot_chunk_size_ok_6436, manifest_entry_count_ok_6437,
 * dlc_size_pack_6438, depot_manifest_size_gate_6439, batch_id_6440).
 * Unique gj_depot_manifest_max_size_6435 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6435_marker[] = "libcgj-batch6435";

/* Soft maximum depot/DLC manifest blob size (256 MiB). */
#define B6435_MFST_MAX  268435456u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6435_max_size(void)
{
	return B6435_MFST_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_depot_manifest_max_size_6435 - soft depot manifest size ceiling.
 *
 * Always returns 268435456 (256 MiB). Soft product bound only.
 * Does not allocate. Does not call libc. No parent wires.
 */
uint32_t
gj_depot_manifest_max_size_6435(void)
{
	(void)NULL;
	return b6435_max_size();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_depot_manifest_max_size_6435(void)
    __attribute__((alias("gj_depot_manifest_max_size_6435")));
