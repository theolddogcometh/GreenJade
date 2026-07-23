/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6447: workshop item type soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_witem_type_ok_6447(uint32_t item_type);
 *     - Soft workshop item type band: type in [0, 15] → 1; else 0.
 *       Soft pure-data gate only (community/file/collection/etc stand-ins).
 *   uint32_t __gj_witem_type_ok_6447  (alias)
 *   __libcgj_batch6447_marker = "libcgj-batch6447"
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

const char __libcgj_batch6447_marker[] = "libcgj-batch6447";

/* Soft workshop item type band (inclusive): [0, 15]. */
#define B6447_TYPE_MAX  15u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6447_type_ok(uint32_t u32Type)
{
	if (u32Type > B6447_TYPE_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_witem_type_ok_6447 - soft workshop item type validity.
 *
 * item_type: soft type discriminator (0..15 inclusive)
 *
 * Returns 1 when type is in [0, 15], else 0. Pure data only.
 * No parent wires.
 */
uint32_t
gj_witem_type_ok_6447(uint32_t item_type)
{
	(void)NULL;
	return b6447_type_ok(item_type);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_witem_type_ok_6447(uint32_t item_type)
    __attribute__((alias("gj_witem_type_ok_6447")));
