/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6165: systemd unit load-state code ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_unit_load_state_ok_6165(uint32_t load_state);
 *     - Returns 1 if load_state is in 0..6 (soft LoadState code range),
 *       else 0.
 *   uint32_t __gj_sd_unit_load_state_ok_6165  (alias)
 *   __libcgj_batch6165_marker = "libcgj-batch6165"
 *
 * Exclusive continuum CREATE-ONLY (6161-6170: systemd unit state
 * stubs — sd_unit_active_state_max_6161, sd_unit_active_state_ok_6162,
 * sd_unit_is_active_6163, sd_unit_is_failed_6164,
 * sd_unit_load_state_ok_6165, sd_unit_is_loaded_6166,
 * sd_unit_state_pack_6167, sd_unit_state_active_of_6168,
 * sd_unit_state_load_of_6169, batch_id / wave_ready_6170).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 *
 * Soft LoadState codes (integer stubs, not D-Bus strings):
 *   0 stub, 1 loaded, 2 not-found, 3 bad-setting,
 *   4 error, 5 merged, 6 masked
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6165_marker[] = "libcgj-batch6165";

#define B6165_LOAD_STATE_MAX  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6165_load_ok(uint32_t u32State)
{
	return (u32State <= B6165_LOAD_STATE_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_unit_load_state_ok_6165 - 1 if LoadState code is in range.
 *
 * load_state: integer LoadState stub code (0..6 valid).
 * Soft bounds only; does not query systemd. No parent wires.
 */
uint32_t
gj_sd_unit_load_state_ok_6165(uint32_t u32LoadState)
{
	(void)NULL;
	return b6165_load_ok(u32LoadState);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sd_unit_load_state_ok_6165(uint32_t u32LoadState)
    __attribute__((alias("gj_sd_unit_load_state_ok_6165")));
