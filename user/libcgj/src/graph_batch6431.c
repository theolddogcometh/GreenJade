/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6431: DLC id soft-ok stub.
 *
 * Surface (unique symbols):
 *   int gj_dlc_id_ok_6431(uint32_t dlc_id);
 *     - Return 1 if dlc_id is in [1, 0xfffffffe] (plausible Steam-style
 *       DLC / package id), else 0. Reserved 0 and all-bits-one → 0.
 *   int __gj_dlc_id_ok_6431  (alias)
 *   __libcgj_batch6431_marker = "libcgj-batch6431"
 *
 * Exclusive continuum CREATE-ONLY (6431-6440: DLC/depot manifest size
 * stubs — dlc_id_ok_6431, depot_id_ok_6432, manifest_size_ok_6433,
 * dlc_manifest_hdr_size_6434, depot_manifest_max_size_6435,
 * depot_chunk_size_ok_6436, manifest_entry_count_ok_6437,
 * dlc_size_pack_6438, depot_manifest_size_gate_6439, batch_id_6440).
 * Unique gj_dlc_id_ok_6431 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6431_marker[] = "libcgj-batch6431";

/* Soft DLC id band: exclude 0 and 0xffffffff. */
#define B6431_DLC_ID_MIN  1u
#define B6431_DLC_ID_MAX  0xfffffffeu

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6431_dlc_ok(uint32_t uDlc)
{
	if (uDlc < B6431_DLC_ID_MIN) {
		return 0;
	}
	if (uDlc > B6431_DLC_ID_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dlc_id_ok_6431 - soft DLC identifier validity gate.
 *
 * dlc_id: candidate DLC / package identifier.
 * Returns 1 when dlc_id is in [1, 0xfffffffe], else 0.
 * Does not consult storefront APIs. Does not call libc. No parent wires.
 */
int
gj_dlc_id_ok_6431(uint32_t uDlcId)
{
	(void)NULL;
	return b6431_dlc_ok(uDlcId);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dlc_id_ok_6431(uint32_t uDlcId)
    __attribute__((alias("gj_dlc_id_ok_6431")));
