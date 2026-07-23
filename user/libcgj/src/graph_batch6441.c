/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6441: workshop item id soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_witem_id_ok_6441(uint32_t item_id);
 *     - Soft non-zero workshop item-id check: 0 → 0; else 1 when id is
 *       in [1, 0xfffffffe]. Reserved all-bits-one (0xffffffff) → 0.
 *   uint32_t __gj_witem_id_ok_6441  (alias)
 *   __libcgj_batch6441_marker = "libcgj-batch6441"
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

const char __libcgj_batch6441_marker[] = "libcgj-batch6441";

#define B6441_ID_MAX  0xfffffffeu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6441_id_ok(uint32_t u32Id)
{
	if (u32Id == 0u || u32Id > B6441_ID_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_witem_id_ok_6441 - soft workshop item id validity.
 *
 * item_id: opaque 32-bit workshop item handle (soft lo-half stand-in)
 *
 * Returns 1 when id is in [1, 0xfffffffe], else 0. Pure data only.
 * No parent wires.
 */
uint32_t
gj_witem_id_ok_6441(uint32_t item_id)
{
	(void)NULL;
	return b6441_id_ok(item_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_witem_id_ok_6441(uint32_t item_id)
    __attribute__((alias("gj_witem_id_ok_6441")));
