/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6439: depot/DLC manifest size aggregate gate.
 *
 * Surface (unique symbols):
 *   int gj_depot_manifest_size_gate_6439(uint32_t mask);
 *     - Return 1 if bits 0..3 of mask are all set (dlc, depot, size,
 *       chunk soft stages), else 0. Soft pure-data aggregate gate only.
 *   int __gj_depot_manifest_size_gate_6439  (alias)
 *   __libcgj_batch6439_marker = "libcgj-batch6439"
 *
 * Exclusive continuum CREATE-ONLY (6431-6440: DLC/depot manifest size
 * stubs — dlc_id_ok_6431, depot_id_ok_6432, manifest_size_ok_6433,
 * dlc_manifest_hdr_size_6434, depot_manifest_max_size_6435,
 * depot_chunk_size_ok_6436, manifest_entry_count_ok_6437,
 * dlc_size_pack_6438, depot_manifest_size_gate_6439, batch_id_6440).
 * Unique gj_depot_manifest_size_gate_6439 surface only; no multi-def.
 * Self-contained; does not call sibling helpers. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6439_marker[] = "libcgj-batch6439";

/*
 * Required soft stage bits for depot/DLC manifest size aggregate pass:
 *   bit0 dlc_ok, bit1 depot_ok, bit2 size_ok, bit3 chunk_ok.
 */
#define B6439_GATE_MASK  0xfu

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6439_gate(uint32_t uMask)
{
	if ((uMask & B6439_GATE_MASK) != B6439_GATE_MASK) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_depot_manifest_size_gate_6439 - soft aggregate size-stage gate.
 *
 * mask: bitfield of DLC/depot manifest size soft stages (bits 0..3).
 * Returns 1 when (mask & 0xf) == 0xf, else 0.
 * Self-contained; does not call sibling helpers. No parent wires.
 */
int
gj_depot_manifest_size_gate_6439(uint32_t uMask)
{
	(void)NULL;
	return b6439_gate(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_depot_manifest_size_gate_6439(uint32_t uMask)
    __attribute__((alias("gj_depot_manifest_size_gate_6439")));
