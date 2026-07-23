/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6443: workshop item id low-half soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_witem_id_lo_ok_6443(uint32_t id_lo);
 *     - Soft low-half workshop item-id check: 0 → 0; else 1 when lo is
 *       in [1, 0xfffffffe]. Reserved all-bits-one → 0.
 *   uint32_t __gj_witem_id_lo_ok_6443  (alias)
 *   __libcgj_batch6443_marker = "libcgj-batch6443"
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

const char __libcgj_batch6443_marker[] = "libcgj-batch6443";

#define B6443_LO_MAX  0xfffffffeu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6443_lo_ok(uint32_t u32Lo)
{
	if (u32Lo == 0u || u32Lo > B6443_LO_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_witem_id_lo_ok_6443 - soft workshop item id low-half validity.
 *
 * id_lo: low 32 bits of a soft 64-bit workshop item id pair
 *
 * Returns 1 when lo is in [1, 0xfffffffe], else 0. Pure data only.
 * No parent wires.
 */
uint32_t
gj_witem_id_lo_ok_6443(uint32_t id_lo)
{
	(void)NULL;
	return b6443_lo_ok(id_lo);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_witem_id_lo_ok_6443(uint32_t id_lo)
    __attribute__((alias("gj_witem_id_lo_ok_6443")));
