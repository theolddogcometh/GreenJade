/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6157: NetworkManager stub device ACTIVATED.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nm_dev_state_activated_6157(void);
 *     - Return the NM device-state code for ACTIVATED (100).
 *       Soft freestanding stub constant; no D-Bus, no libnm.
 *   uint32_t __gj_nm_dev_state_activated_6157  (alias)
 *   __libcgj_batch6157_marker = "libcgj-batch6157"
 *
 * Exclusive continuum CREATE-ONLY network manager stub states wave
 * (6151-6160: nm_mgr_state_unknown_6151, nm_mgr_state_asleep_6152,
 * nm_mgr_state_disconnected_6153, nm_mgr_state_connecting_6154,
 * nm_mgr_state_connected_local_6155, nm_mgr_state_connected_global_6156,
 * nm_dev_state_activated_6157, nm_connectivity_full_6158,
 * nm_mgr_is_connected_6159, batch_id_6160). Unique
 * gj_nm_dev_state_activated_6157 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6157_marker[] = "libcgj-batch6157";

/* NM_DEVICE_STATE_ACTIVATED (classic NetworkManager device-state ladder). */
#define B6157_NM_DEVICE_STATE_ACTIVATED  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6157_activated(void)
{
	return B6157_NM_DEVICE_STATE_ACTIVATED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nm_dev_state_activated_6157 - report NM device ACTIVATED state code.
 *
 * Always returns 100. Soft compile-time stub constant for NM device state.
 * No parent wires.
 */
uint32_t
gj_nm_dev_state_activated_6157(void)
{
	(void)NULL;
	return b6157_activated();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nm_dev_state_activated_6157(void)
    __attribute__((alias("gj_nm_dev_state_activated_6157")));
