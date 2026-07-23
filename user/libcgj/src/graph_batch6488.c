/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6488: friend presence app-id soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fpres_appid_ok_6488(uint32_t app_id);
 *     - Soft non-zero currently-playing app id: 0 → 0; else 1 when
 *       app_id is in [1, 0xfffffffe]. Reserved 0xffffffff → 0.
 *   uint32_t __gj_fpres_appid_ok_6488  (alias)
 *   __libcgj_batch6488_marker = "libcgj-batch6488"
 *
 * Exclusive continuum CREATE-ONLY (6481-6490: friend presence state stubs
 * — fpres_uid_ok_6481, fpres_state_ok_6482, fpres_online_p_6483,
 * fpres_away_p_6484, fpres_busy_p_6485, fpres_ingame_p_6486,
 * fpres_last_seen_ok_6487, fpres_appid_ok_6488, fpres_gate_6489,
 * batch_id_6490). Unique gj_fpres_appid_ok_6488 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6488_marker[] = "libcgj-batch6488";

#define B6488_APPID_MAX  0xfffffffeu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6488_appid_ok(uint32_t u32App)
{
	if (u32App == 0u || u32App > B6488_APPID_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fpres_appid_ok_6488 - soft currently-playing app id validity.
 *
 * app_id: opaque 32-bit application identity for presence "in game"
 *
 * Returns 1 when app_id is in [1, 0xfffffffe], else 0. Pure data only.
 * No parent wires.
 */
uint32_t
gj_fpres_appid_ok_6488(uint32_t app_id)
{
	(void)NULL;
	return b6488_appid_ok(app_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fpres_appid_ok_6488(uint32_t app_id)
    __attribute__((alias("gj_fpres_appid_ok_6488")));
