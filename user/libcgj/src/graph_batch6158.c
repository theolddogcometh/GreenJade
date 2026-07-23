/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6158: NetworkManager stub connectivity FULL.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nm_connectivity_full_6158(void);
 *     - Return the NM connectivity-state code for FULL (4).
 *       Soft freestanding stub constant; no D-Bus, no libnm.
 *   uint32_t __gj_nm_connectivity_full_6158  (alias)
 *   __libcgj_batch6158_marker = "libcgj-batch6158"
 *
 * Exclusive continuum CREATE-ONLY network manager stub states wave
 * (6151-6160: nm_mgr_state_unknown_6151, nm_mgr_state_asleep_6152,
 * nm_mgr_state_disconnected_6153, nm_mgr_state_connecting_6154,
 * nm_mgr_state_connected_local_6155, nm_mgr_state_connected_global_6156,
 * nm_dev_state_activated_6157, nm_connectivity_full_6158,
 * nm_mgr_is_connected_6159, batch_id_6160). Unique
 * gj_nm_connectivity_full_6158 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6158_marker[] = "libcgj-batch6158";

/* NM_CONNECTIVITY_FULL (classic NetworkManager connectivity ladder). */
#define B6158_NM_CONNECTIVITY_FULL  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6158_full(void)
{
	return B6158_NM_CONNECTIVITY_FULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nm_connectivity_full_6158 - report NM connectivity FULL code.
 *
 * Always returns 4. Soft compile-time stub constant for NM connectivity.
 * No parent wires.
 */
uint32_t
gj_nm_connectivity_full_6158(void)
{
	(void)NULL;
	return b6158_full();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nm_connectivity_full_6158(void)
    __attribute__((alias("gj_nm_connectivity_full_6158")));
