/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9431: NetworkManager soft WIFI device type id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nm_device_wifi_id_9431(void);
 *     - Return the classic NM device-type code for WIFI (2).
 *       Soft freestanding stub constant; no D-Bus, no libnm.
 *   uint32_t __gj_nm_device_wifi_id_9431  (alias)
 *   __libcgj_batch9431_marker = "libcgj-batch9431"
 *
 * Exclusive continuum CREATE-ONLY networkmanager soft id stubs wave
 * (9431-9440: nm_device_wifi_id_9431, nm_device_eth_id_9432,
 * nm_device_vpn_id_9433, nm_state_connected_id_9434,
 * nm_state_disconnected_id_9435, nm_ap_ok_u_9436, nm_conn_ok_u_9437,
 * nm_settings_ok_u_9438, nm_ready_u_9439, batch_id_9440). Unique
 * gj_nm_device_wifi_id_9431 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9431_marker[] = "libcgj-batch9431";

/* NM_DEVICE_TYPE_WIFI (classic NetworkManager device-type ladder). */
#define B9431_NM_DEVICE_TYPE_WIFI  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9431_wifi_id(void)
{
	return B9431_NM_DEVICE_TYPE_WIFI;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nm_device_wifi_id_9431 - report classic WIFI device type id.
 *
 * Always returns 2. Soft compile-time catalog id for NM WIFI devices.
 * No parent wires.
 */
uint32_t
gj_nm_device_wifi_id_9431(void)
{
	(void)NULL;
	return b9431_wifi_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nm_device_wifi_id_9431(void)
    __attribute__((alias("gj_nm_device_wifi_id_9431")));
