/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6487: friend presence last-seen age soft-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fpres_last_seen_ok_6487(uint32_t age_sec,
 *                                       uint32_t max_age_sec);
 *     - Soft last-seen window: age_sec <= max_age_sec → 1; else 0.
 *       max_age_sec of 0 uses default 86400 (24h soft window).
 *   uint32_t __gj_fpres_last_seen_ok_6487  (alias)
 *   __libcgj_batch6487_marker = "libcgj-batch6487"
 *
 * Exclusive continuum CREATE-ONLY (6481-6490: friend presence state stubs
 * — fpres_uid_ok_6481, fpres_state_ok_6482, fpres_online_p_6483,
 * fpres_away_p_6484, fpres_busy_p_6485, fpres_ingame_p_6486,
 * fpres_last_seen_ok_6487, fpres_appid_ok_6488, fpres_gate_6489,
 * batch_id_6490). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6487_marker[] = "libcgj-batch6487";

#define B6487_MAX_AGE_DEFAULT  86400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6487_last_seen_ok(uint32_t u32Age, uint32_t u32MaxAge)
{
	uint32_t u32Cap;

	u32Cap = u32MaxAge;
	if (u32Cap == 0u) {
		u32Cap = B6487_MAX_AGE_DEFAULT;
	}
	if (u32Age > u32Cap) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fpres_last_seen_ok_6487 - soft last-seen age window gate.
 *
 * age_sec:     seconds since last soft presence observation
 * max_age_sec: allowed window (0 → default 86400)
 *
 * Returns 1 when age_sec <= max window, else 0. Pure integer.
 * No parent wires.
 */
uint32_t
gj_fpres_last_seen_ok_6487(uint32_t age_sec, uint32_t max_age_sec)
{
	(void)NULL;
	return b6487_last_seen_ok(age_sec, max_age_sec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fpres_last_seen_ok_6487(uint32_t age_sec, uint32_t max_age_sec)
    __attribute__((alias("gj_fpres_last_seen_ok_6487")));
