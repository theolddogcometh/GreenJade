/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6482: friend presence state enum soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fpres_state_ok_6482(uint32_t state);
 *     - Return 1 if state is in [0, 4] soft enum band:
 *       0 offline, 1 online, 2 busy, 3 away, 4 snooze; else 0.
 *   uint32_t __gj_fpres_state_ok_6482  (alias)
 *   __libcgj_batch6482_marker = "libcgj-batch6482"
 *
 * Exclusive continuum CREATE-ONLY (6481-6490: friend presence state stubs
 * — fpres_uid_ok_6481, fpres_state_ok_6482, fpres_online_p_6483,
 * fpres_away_p_6484, fpres_busy_p_6485, fpres_ingame_p_6486,
 * fpres_last_seen_ok_6487, fpres_appid_ok_6488, fpres_gate_6489,
 * batch_id_6490). Unique gj_fpres_state_ok_6482 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6482_marker[] = "libcgj-batch6482";

/* Soft presence state band (inclusive): [0, 4]. */
#define B6482_STATE_MAX  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6482_state_ok(uint32_t u32State)
{
	if (u32State > B6482_STATE_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fpres_state_ok_6482 - soft friend presence state enum validity.
 *
 * state: soft presence state code (0..4).
 * Returns 1 when state is in [0, 4], else 0.
 * Does not map state names. Does not call libc. No parent wires.
 */
uint32_t
gj_fpres_state_ok_6482(uint32_t state)
{
	(void)NULL;
	return b6482_state_ok(state);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fpres_state_ok_6482(uint32_t state)
    __attribute__((alias("gj_fpres_state_ok_6482")));
