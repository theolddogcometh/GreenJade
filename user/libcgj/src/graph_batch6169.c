/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6169: extract LoadState from packed unit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_unit_state_load_of_6169(uint32_t packed);
 *     - Returns bits 8..15 of packed (LoadState field from
 *       gj_sd_unit_state_pack_6167 layout). Soft extract only.
 *   uint32_t __gj_sd_unit_state_load_of_6169  (alias)
 *   __libcgj_batch6169_marker = "libcgj-batch6169"
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

const char __libcgj_batch6169_marker[] = "libcgj-batch6169";

#define B6169_BYTE_MAX  0xffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6169_load_of(uint32_t u32Packed)
{
	return (u32Packed >> 8) & B6169_BYTE_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_unit_state_load_of_6169 - LoadState field from packed word.
 *
 * packed: word from gj_sd_unit_state_pack_6167 layout.
 * Soft extract only; no D-Bus. No parent wires.
 */
uint32_t
gj_sd_unit_state_load_of_6169(uint32_t u32Packed)
{
	(void)NULL;
	return b6169_load_of(u32Packed);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sd_unit_state_load_of_6169(uint32_t u32Packed)
    __attribute__((alias("gj_sd_unit_state_load_of_6169")));
