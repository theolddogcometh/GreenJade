/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6448: workshop item visibility soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_witem_vis_ok_6448(uint32_t visibility);
 *     - Soft visibility band: visibility in [0, 3] → 1; else 0
 *       (public / friends / private / unlisted soft stand-ins).
 *   uint32_t __gj_witem_vis_ok_6448  (alias)
 *   __libcgj_batch6448_marker = "libcgj-batch6448"
 *
 * Exclusive continuum CREATE-ONLY (6441-6450: workshop item id stubs —
 * witem_id_ok_6441, witem_id_hi_ok_6442, witem_id_lo_ok_6443,
 * witem_id_pair_ok_6444, witem_id_pack_6445, witem_appid_ok_6446,
 * witem_type_ok_6447, witem_vis_ok_6448, witem_id_gate_6449,
 * batch_id_6450). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6448_marker[] = "libcgj-batch6448";

/* Soft visibility band (inclusive): [0, 3]. */
#define B6448_VIS_MAX  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6448_vis_ok(uint32_t u32Vis)
{
	if (u32Vis > B6448_VIS_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_witem_vis_ok_6448 - soft workshop item visibility validity.
 *
 * visibility: soft visibility code (0..3 inclusive)
 *
 * Returns 1 when visibility is in [0, 3], else 0. Pure data only.
 * No parent wires.
 */
uint32_t
gj_witem_vis_ok_6448(uint32_t visibility)
{
	(void)NULL;
	return b6448_vis_ok(visibility);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_witem_vis_ok_6448(uint32_t visibility)
    __attribute__((alias("gj_witem_vis_ok_6448")));
