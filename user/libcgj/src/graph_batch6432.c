/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6432: depot id soft-ok stub (suffixed).
 *
 * Surface (unique symbols):
 *   int gj_depot_id_ok_6432(uint32_t depot_id);
 *     - Return 1 if depot_id is in [1, 0xfffffffe] (plausible Steam
 *       depot id band for manifest size work), else 0.
 *   int __gj_depot_id_ok_6432  (alias)
 *   __libcgj_batch6432_marker = "libcgj-batch6432"
 *
 * Exclusive continuum CREATE-ONLY (6431-6440: DLC/depot manifest size
 * stubs — dlc_id_ok_6431, depot_id_ok_6432, manifest_size_ok_6433,
 * dlc_manifest_hdr_size_6434, depot_manifest_max_size_6435,
 * depot_chunk_size_ok_6436, manifest_entry_count_ok_6437,
 * dlc_size_pack_6438, depot_manifest_size_gate_6439, batch_id_6440).
 * Distinct from gj_depot_id_ok (batch1862). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6432_marker[] = "libcgj-batch6432";

/* Soft depot id band: exclude 0 and 0xffffffff. */
#define B6432_DEPOT_ID_MIN  1u
#define B6432_DEPOT_ID_MAX  0xfffffffeu

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6432_depot_ok(uint32_t uDepot)
{
	if (uDepot < B6432_DEPOT_ID_MIN) {
		return 0;
	}
	if (uDepot > B6432_DEPOT_ID_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_depot_id_ok_6432 - soft depot identifier validity gate.
 *
 * depot_id: candidate content depot identifier.
 * Returns 1 when depot_id is in [1, 0xfffffffe], else 0.
 * Does not open depotcache. Does not call libc. No parent wires.
 */
int
gj_depot_id_ok_6432(uint32_t uDepotId)
{
	(void)NULL;
	return b6432_depot_ok(uDepotId);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_depot_id_ok_6432(uint32_t uDepotId)
    __attribute__((alias("gj_depot_id_ok_6432")));
