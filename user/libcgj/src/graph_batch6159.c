/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6159: NetworkManager stub "is connected" probe.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nm_mgr_is_connected_6159(uint32_t state);
 *     - Return 1u if state is a CONNECTED_* manager overall state
 *       (CONNECTED_LOCAL 50, CONNECTED_SITE 60, CONNECTED_GLOBAL 70);
 *       else 0u. Soft freestanding stub classifier; no D-Bus, no libnm.
 *   uint32_t __gj_nm_mgr_is_connected_6159  (alias)
 *   __libcgj_batch6159_marker = "libcgj-batch6159"
 *
 * Exclusive continuum CREATE-ONLY network manager stub states wave
 * (6151-6160: nm_mgr_state_unknown_6151, nm_mgr_state_asleep_6152,
 * nm_mgr_state_disconnected_6153, nm_mgr_state_connecting_6154,
 * nm_mgr_state_connected_local_6155, nm_mgr_state_connected_global_6156,
 * nm_dev_state_activated_6157, nm_connectivity_full_6158,
 * nm_mgr_is_connected_6159, batch_id_6160). Unique
 * gj_nm_mgr_is_connected_6159 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6159_marker[] = "libcgj-batch6159";

/* Classic NM overall-state CONNECTED_* band (inclusive). */
#define B6159_NM_STATE_CONNECTED_LOCAL   50u
#define B6159_NM_STATE_CONNECTED_GLOBAL  70u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6159_is_connected(uint32_t uState)
{
	if (uState >= B6159_NM_STATE_CONNECTED_LOCAL &&
	    uState <= B6159_NM_STATE_CONNECTED_GLOBAL) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nm_mgr_is_connected_6159 - 1u if manager state is CONNECTED_*.
 *
 * state: NM overall-state code. Accepts the classic CONNECTED_LOCAL (50),
 * CONNECTED_SITE (60), and CONNECTED_GLOBAL (70) band. No parent wires.
 */
uint32_t
gj_nm_mgr_is_connected_6159(uint32_t state)
{
	(void)NULL;
	return b6159_is_connected(state);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nm_mgr_is_connected_6159(uint32_t state)
    __attribute__((alias("gj_nm_mgr_is_connected_6159")));
