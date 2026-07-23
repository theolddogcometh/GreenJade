/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6485: friend presence busy predicate stub.
 *
 * Surface (unique symbols):
 *   int gj_fpres_busy_p_6485(uint32_t state);
 *     - Return 1 if state == 2 (soft busy/dnd), else 0.
 *   int __gj_fpres_busy_p_6485  (alias)
 *   __libcgj_batch6485_marker = "libcgj-batch6485"
 *
 * Exclusive continuum CREATE-ONLY (6481-6490: friend presence state stubs
 * — fpres_uid_ok_6481, fpres_state_ok_6482, fpres_online_p_6483,
 * fpres_away_p_6484, fpres_busy_p_6485, fpres_ingame_p_6486,
 * fpres_last_seen_ok_6487, fpres_appid_ok_6488, fpres_gate_6489,
 * batch_id_6490). Unique gj_fpres_busy_p_6485 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6485_marker[] = "libcgj-batch6485";

#define B6485_STATE_BUSY  2u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6485_busy_p(uint32_t u32State)
{
	if (u32State == B6485_STATE_BUSY) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fpres_busy_p_6485 - soft friend presence busy/dnd predicate.
 *
 * state: soft presence state code (2 = busy).
 * Returns 1 when state == 2, else 0.
 * Soft pure-data only. No parent wires.
 */
int
gj_fpres_busy_p_6485(uint32_t state)
{
	(void)NULL;
	return b6485_busy_p(state);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fpres_busy_p_6485(uint32_t state)
    __attribute__((alias("gj_fpres_busy_p_6485")));
