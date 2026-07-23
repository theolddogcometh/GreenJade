/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6449: workshop item id composite soft gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_witem_id_gate_6449(uint32_t id_ok, uint32_t app_ok,
 *                                  uint32_t type_ok, uint32_t vis_ok);
 *     - Soft composite gate: all of id_ok, app_ok, type_ok, vis_ok must
 *       be non-zero → 1; else 0.
 *   uint32_t __gj_witem_id_gate_6449  (alias)
 *   __libcgj_batch6449_marker = "libcgj-batch6449"
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

const char __libcgj_batch6449_marker[] = "libcgj-batch6449";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6449_gate(uint32_t u32IdOk, uint32_t u32AppOk, uint32_t u32TypeOk,
    uint32_t u32VisOk)
{
	if (u32IdOk == 0u || u32AppOk == 0u || u32TypeOk == 0u
	    || u32VisOk == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_witem_id_gate_6449 - soft workshop item id composite gate.
 *
 * id_ok:   non-zero when item id pair soft-ok
 * app_ok:  non-zero when owning appid soft-ok
 * type_ok: non-zero when item type soft-ok
 * vis_ok:  non-zero when visibility soft-ok
 *
 * Returns 1 when all gates pass, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_witem_id_gate_6449(uint32_t id_ok, uint32_t app_ok, uint32_t type_ok,
    uint32_t vis_ok)
{
	(void)NULL;
	return b6449_gate(id_ok, app_ok, type_ok, vis_ok);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_witem_id_gate_6449(uint32_t id_ok, uint32_t app_ok,
    uint32_t type_ok, uint32_t vis_ok)
    __attribute__((alias("gj_witem_id_gate_6449")));
