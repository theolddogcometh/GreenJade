/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6161: systemd unit active-state code max.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_unit_active_state_max_6161(void);
 *     - Returns the soft max valid systemd unit ActiveState code
 *       (6 = maintenance) for integer-only unit state stubs.
 *   uint32_t __gj_sd_unit_active_state_max_6161  (alias)
 *   __libcgj_batch6161_marker = "libcgj-batch6161"
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

const char __libcgj_batch6161_marker[] = "libcgj-batch6161";

/*
 * Soft ActiveState codes (integer stubs, not D-Bus strings):
 *   0 inactive, 1 active, 2 reloading, 3 failed,
 *   4 activating, 5 deactivating, 6 maintenance
 */
#define B6161_ACTIVE_STATE_MAX  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6161_active_max(void)
{
	return B6161_ACTIVE_STATE_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_unit_active_state_max_6161 - soft max ActiveState code.
 *
 * Always returns 6. Compile-time bound for freestanding systemd unit
 * active-state validity checks; does not talk to systemd. No parent wires.
 */
uint32_t
gj_sd_unit_active_state_max_6161(void)
{
	(void)NULL;
	return b6161_active_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sd_unit_active_state_max_6161(void)
    __attribute__((alias("gj_sd_unit_active_state_max_6161")));
