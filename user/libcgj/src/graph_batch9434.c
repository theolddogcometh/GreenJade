/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9434: NetworkManager soft CONNECTED state id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nm_state_connected_id_9434(void);
 *     - Return the classic NM manager overall-state code for
 *       CONNECTED_GLOBAL (70). Soft freestanding stub constant;
 *       no D-Bus, no libnm.
 *   uint32_t __gj_nm_state_connected_id_9434  (alias)
 *   __libcgj_batch9434_marker = "libcgj-batch9434"
 *
 * Exclusive continuum CREATE-ONLY networkmanager soft id stubs wave
 * (9431-9440: nm_device_wifi_id_9431, nm_device_eth_id_9432,
 * nm_device_vpn_id_9433, nm_state_connected_id_9434,
 * nm_state_disconnected_id_9435, nm_ap_ok_u_9436, nm_conn_ok_u_9437,
 * nm_settings_ok_u_9438, nm_ready_u_9439, batch_id_9440). Unique
 * gj_nm_state_connected_id_9434 surface only; no multi-def.
 * Distinct from gj_nm_mgr_state_connected_global_6156 (batch6156).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9434_marker[] = "libcgj-batch9434";

/* NM_STATE_CONNECTED_GLOBAL (classic NetworkManager overall-state ladder). */
#define B9434_NM_STATE_CONNECTED  70u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9434_connected_id(void)
{
	return B9434_NM_STATE_CONNECTED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nm_state_connected_id_9434 - report CONNECTED_GLOBAL state id.
 *
 * Always returns 70. Soft compile-time catalog id for full NM connectivity.
 * No parent wires.
 */
uint32_t
gj_nm_state_connected_id_9434(void)
{
	(void)NULL;
	return b9434_connected_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nm_state_connected_id_9434(void)
    __attribute__((alias("gj_nm_state_connected_id_9434")));
