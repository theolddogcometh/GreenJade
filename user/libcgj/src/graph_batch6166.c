/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6166: systemd unit is-loaded state test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_unit_is_loaded_6166(uint32_t load_state);
 *     - Returns 1 if load_state == 1 (loaded), else 0.
 *   uint32_t __gj_sd_unit_is_loaded_6166  (alias)
 *   __libcgj_batch6166_marker = "libcgj-batch6166"
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

const char __libcgj_batch6166_marker[] = "libcgj-batch6166";

#define B6166_LOADED  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6166_is_loaded(uint32_t u32State)
{
	return (u32State == B6166_LOADED) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_unit_is_loaded_6166 - 1 if unit LoadState is "loaded".
 *
 * load_state: integer LoadState stub (1 = loaded).
 * Soft pure-data test only. No parent wires.
 */
uint32_t
gj_sd_unit_is_loaded_6166(uint32_t u32LoadState)
{
	(void)NULL;
	return b6166_is_loaded(u32LoadState);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sd_unit_is_loaded_6166(uint32_t u32LoadState)
    __attribute__((alias("gj_sd_unit_is_loaded_6166")));
