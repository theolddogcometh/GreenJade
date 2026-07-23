/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6442: workshop item id high-half soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_witem_id_hi_ok_6442(uint32_t id_hi);
 *     - Soft high-half workshop item-id check: any value except the
 *       reserved all-bits-one sentinel (0xffffffff) → 1; 0xffffffff → 0.
 *       Zero high half is allowed (fits soft 32-bit item ids).
 *   uint32_t __gj_witem_id_hi_ok_6442  (alias)
 *   __libcgj_batch6442_marker = "libcgj-batch6442"
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

const char __libcgj_batch6442_marker[] = "libcgj-batch6442";

#define B6442_HI_RESERVED  0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6442_hi_ok(uint32_t u32Hi)
{
	if (u32Hi == B6442_HI_RESERVED) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_witem_id_hi_ok_6442 - soft workshop item id high-half validity.
 *
 * id_hi: high 32 bits of a soft 64-bit workshop item id pair
 *
 * Returns 1 when id_hi != 0xffffffff, else 0. Pure data only.
 * No parent wires.
 */
uint32_t
gj_witem_id_hi_ok_6442(uint32_t id_hi)
{
	(void)NULL;
	return b6442_hi_ok(id_hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_witem_id_hi_ok_6442(uint32_t id_hi)
    __attribute__((alias("gj_witem_id_hi_ok_6442")));
