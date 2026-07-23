/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6445: workshop item id soft pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_witem_id_pack_6445(uint32_t id_hi, uint32_t id_lo);
 *     - Soft fold of hi/lo halves into a 32-bit mix (xor + rotate-ish
 *       nibble shuffle). Does not allocate. Pure integer mix only.
 *   uint32_t __gj_witem_id_pack_6445  (alias)
 *   __libcgj_batch6445_marker = "libcgj-batch6445"
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

const char __libcgj_batch6445_marker[] = "libcgj-batch6445";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6445_pack(uint32_t u32Hi, uint32_t u32Lo)
{
	uint32_t uMix;

	/* Soft mix: xor then rotate left by 7 via shift/or (no rol). */
	uMix = u32Hi ^ u32Lo;
	uMix = (uMix << 7) | (uMix >> 25);
	uMix ^= (u32Lo << 3) | (u32Hi >> 5);
	return uMix;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_witem_id_pack_6445 - soft workshop item id hi/lo pack.
 *
 * id_hi: high 32 bits of soft workshop item id
 * id_lo: low 32 bits of soft workshop item id
 *
 * Returns a pure-integer mix of the halves. Not a cryptographic hash.
 * No parent wires.
 */
uint32_t
gj_witem_id_pack_6445(uint32_t id_hi, uint32_t id_lo)
{
	(void)NULL;
	return b6445_pack(id_hi, id_lo);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_witem_id_pack_6445(uint32_t id_hi, uint32_t id_lo)
    __attribute__((alias("gj_witem_id_pack_6445")));
