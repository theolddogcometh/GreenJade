/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6163: systemd unit is-active state test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_unit_is_active_6163(uint32_t active_state);
 *     - Returns 1 if active_state == 1 (active), else 0.
 *   uint32_t __gj_sd_unit_is_active_6163  (alias)
 *   __libcgj_batch6163_marker = "libcgj-batch6163"
 *
 * Exclusive continuum CREATE-ONLY (6161-6170: systemd unit state
 * stubs — sd_unit_active_state_max_6161, sd_unit_active_state_ok_6162,
 * sd_unit_is_active_6163, sd_unit_is_failed_6164,
 * sd_unit_load_state_ok_6165, sd_unit_is_loaded_6166,
 * sd_unit_state_pack_6167, sd_unit_state_active_of_6168,
 * sd_unit_state_load_of_6169, batch_id / wave_ready_6170).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6163_marker[] = "libcgj-batch6163";

#define B6163_ACTIVE  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6163_is_active(uint32_t u32State)
{
	return (u32State == B6163_ACTIVE) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_unit_is_active_6163 - 1 if unit ActiveState is "active".
 *
 * active_state: integer ActiveState stub (1 = active).
 * Soft pure-data test only. No parent wires.
 */
uint32_t
gj_sd_unit_is_active_6163(uint32_t u32ActiveState)
{
	(void)NULL;
	return b6163_is_active(u32ActiveState);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sd_unit_is_active_6163(uint32_t u32ActiveState)
    __attribute__((alias("gj_sd_unit_is_active_6163")));
