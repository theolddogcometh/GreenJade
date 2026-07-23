/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6153: NetworkManager stub manager DISCONNECTED.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nm_mgr_state_disconnected_6153(void);
 *     - Return the NM manager overall-state code for DISCONNECTED (20).
 *       Soft freestanding stub constant; no D-Bus, no libnm.
 *   uint32_t __gj_nm_mgr_state_disconnected_6153  (alias)
 *   __libcgj_batch6153_marker = "libcgj-batch6153"
 *
 * Exclusive continuum CREATE-ONLY network manager stub states wave
 * (6151-6160: nm_mgr_state_unknown_6151, nm_mgr_state_asleep_6152,
 * nm_mgr_state_disconnected_6153, nm_mgr_state_connecting_6154,
 * nm_mgr_state_connected_local_6155, nm_mgr_state_connected_global_6156,
 * nm_dev_state_activated_6157, nm_connectivity_full_6158,
 * nm_mgr_is_connected_6159, batch_id_6160). Unique
 * gj_nm_mgr_state_disconnected_6153 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6153_marker[] = "libcgj-batch6153";

/* NM_STATE_DISCONNECTED (classic NetworkManager overall-state ladder). */
#define B6153_NM_STATE_DISCONNECTED  20u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6153_disconnected(void)
{
	return B6153_NM_STATE_DISCONNECTED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nm_mgr_state_disconnected_6153 - report NM manager DISCONNECTED code.
 *
 * Always returns 20. Soft compile-time stub constant for NM overall state.
 * No parent wires.
 */
uint32_t
gj_nm_mgr_state_disconnected_6153(void)
{
	(void)NULL;
	return b6153_disconnected();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nm_mgr_state_disconnected_6153(void)
    __attribute__((alias("gj_nm_mgr_state_disconnected_6153")));
