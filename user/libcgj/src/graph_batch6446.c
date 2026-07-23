/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6446: workshop item appid soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_witem_appid_ok_6446(uint32_t appid);
 *     - Soft app-id band for workshop association: appid in
 *       [1, 0x7fffffff] → 1; else 0. Soft pure-data gate only.
 *   uint32_t __gj_witem_appid_ok_6446  (alias)
 *   __libcgj_batch6446_marker = "libcgj-batch6446"
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

const char __libcgj_batch6446_marker[] = "libcgj-batch6446";

#define B6446_APPID_MAX  0x7fffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6446_appid_ok(uint32_t u32App)
{
	if (u32App == 0u || u32App > B6446_APPID_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_witem_appid_ok_6446 - soft workshop item owning-appid validity.
 *
 * appid: soft application id associated with a workshop item
 *
 * Returns 1 when appid is in [1, 0x7fffffff], else 0. Pure data only.
 * No parent wires.
 */
uint32_t
gj_witem_appid_ok_6446(uint32_t appid)
{
	(void)NULL;
	return b6446_appid_ok(appid);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_witem_appid_ok_6446(uint32_t appid)
    __attribute__((alias("gj_witem_appid_ok_6446")));
