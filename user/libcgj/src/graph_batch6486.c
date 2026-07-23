/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6486: friend presence in-game predicate stub.
 *
 * Surface (unique symbols):
 *   int gj_fpres_ingame_p_6486(uint32_t state, uint32_t app_flag);
 *     - Return 1 if state is a soft "present" band [1, 4] and app_flag
 *       is non-zero (friend soft-reported as in-game), else 0.
 *   int __gj_fpres_ingame_p_6486  (alias)
 *   __libcgj_batch6486_marker = "libcgj-batch6486"
 *
 * Exclusive continuum CREATE-ONLY (6481-6490: friend presence state stubs
 * — fpres_uid_ok_6481, fpres_state_ok_6482, fpres_online_p_6483,
 * fpres_away_p_6484, fpres_busy_p_6485, fpres_ingame_p_6486,
 * fpres_last_seen_ok_6487, fpres_appid_ok_6488, fpres_gate_6489,
 * batch_id_6490). Unique gj_fpres_ingame_p_6486 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6486_marker[] = "libcgj-batch6486";

#define B6486_STATE_ONLINE  1u
#define B6486_STATE_MAX     4u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6486_ingame_p(uint32_t u32State, uint32_t u32AppFlag)
{
	if (u32AppFlag == 0u) {
		return 0;
	}
	if (u32State < B6486_STATE_ONLINE || u32State > B6486_STATE_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fpres_ingame_p_6486 - soft friend presence in-game predicate.
 *
 * state:    soft presence state code (need [1, 4] present band)
 * app_flag: non-zero when a currently-playing app is soft-reported
 *
 * Returns 1 when present and app_flag != 0, else 0.
 * Soft pure-data only. No parent wires.
 */
int
gj_fpres_ingame_p_6486(uint32_t state, uint32_t app_flag)
{
	(void)NULL;
	return b6486_ingame_p(state, app_flag);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fpres_ingame_p_6486(uint32_t state, uint32_t app_flag)
    __attribute__((alias("gj_fpres_ingame_p_6486")));
