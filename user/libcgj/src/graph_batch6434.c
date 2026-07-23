/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6434: DLC manifest header size constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dlc_manifest_hdr_size_6434(void);
 *     - Returns the soft fixed DLC/depot manifest header size in bytes
 *       (40). Integer-only layout stub for freestanding size gates.
 *   uint32_t __gj_dlc_manifest_hdr_size_6434  (alias)
 *   __libcgj_batch6434_marker = "libcgj-batch6434"
 *
 * Exclusive continuum CREATE-ONLY (6431-6440: DLC/depot manifest size
 * stubs — dlc_id_ok_6431, depot_id_ok_6432, manifest_size_ok_6433,
 * dlc_manifest_hdr_size_6434, depot_manifest_max_size_6435,
 * depot_chunk_size_ok_6436, manifest_entry_count_ok_6437,
 * dlc_size_pack_6438, depot_manifest_size_gate_6439, batch_id_6440).
 * Unique gj_dlc_manifest_hdr_size_6434 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6434_marker[] = "libcgj-batch6434";

/* Soft fixed header length for DLC/depot manifest size accounting. */
#define B6434_HDR_SIZE  40u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6434_hdr_size(void)
{
	return B6434_HDR_SIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dlc_manifest_hdr_size_6434 - soft DLC/depot manifest header size.
 *
 * Always returns 40 (bytes). Soft layout constant only.
 * Does not read disk. Does not call libc. No parent wires.
 */
uint32_t
gj_dlc_manifest_hdr_size_6434(void)
{
	(void)NULL;
	return b6434_hdr_size();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dlc_manifest_hdr_size_6434(void)
    __attribute__((alias("gj_dlc_manifest_hdr_size_6434")));
