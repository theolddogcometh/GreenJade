/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6167: systemd unit state pack (active|load).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_unit_state_pack_6167(uint32_t active_state,
 *                                       uint32_t load_state);
 *     - Pack soft unit state codes into one word:
 *         bits 0..7  = active_state clamped to 0..0xff
 *         bits 8..15 = load_state clamped to 0..0xff
 *         bits 16..31 = 0
 *   uint32_t __gj_sd_unit_state_pack_6167  (alias)
 *   __libcgj_batch6167_marker = "libcgj-batch6167"
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

const char __libcgj_batch6167_marker[] = "libcgj-batch6167";

#define B6167_BYTE_MAX  0xffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6167_clamp8(uint32_t u32V)
{
	if (u32V > B6167_BYTE_MAX) {
		return B6167_BYTE_MAX;
	}
	return u32V;
}

static uint32_t
b6167_pack(uint32_t u32Active, uint32_t u32Load)
{
	return b6167_clamp8(u32Active) | (b6167_clamp8(u32Load) << 8);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_unit_state_pack_6167 - pack ActiveState + LoadState codes.
 *
 * active_state: ActiveState stub code (clamped to 8 bits)
 * load_state:   LoadState stub code (clamped to 8 bits)
 *
 * Returns packed summary word. Soft pure-data only. No parent wires.
 */
uint32_t
gj_sd_unit_state_pack_6167(uint32_t u32ActiveState, uint32_t u32LoadState)
{
	(void)NULL;
	return b6167_pack(u32ActiveState, u32LoadState);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sd_unit_state_pack_6167(uint32_t u32ActiveState,
    uint32_t u32LoadState)
    __attribute__((alias("gj_sd_unit_state_pack_6167")));
