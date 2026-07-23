/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9433: NetworkManager soft VPN connection type id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nm_device_vpn_id_9433(void);
 *     - Return the soft freestanding VPN connection-type catalog id (100).
 *       Soft compile-time tag; no D-Bus, no libnm. Not a classic
 *       NM_DEVICE_TYPE value (VPN is a connection class, not a device).
 *   uint32_t __gj_nm_device_vpn_id_9433  (alias)
 *   __libcgj_batch9433_marker = "libcgj-batch9433"
 *
 * Exclusive continuum CREATE-ONLY networkmanager soft id stubs wave
 * (9431-9440: nm_device_wifi_id_9431, nm_device_eth_id_9432,
 * nm_device_vpn_id_9433, nm_state_connected_id_9434,
 * nm_state_disconnected_id_9435, nm_ap_ok_u_9436, nm_conn_ok_u_9437,
 * nm_settings_ok_u_9438, nm_ready_u_9439, batch_id_9440). Unique
 * gj_nm_device_vpn_id_9433 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9433_marker[] = "libcgj-batch9433";

/*
 * Soft freestanding VPN connection-type catalog id. Classic NM uses a
 * string connection type ("vpn"); this integer tag is for integer-only
 * freestanding classifiers and is not a classic NM_DEVICE_TYPE.
 */
#define B9433_NM_DEVICE_VPN_ID  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9433_vpn_id(void)
{
	return B9433_NM_DEVICE_VPN_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nm_device_vpn_id_9433 - report soft VPN connection-type catalog id.
 *
 * Always returns 100. Soft compile-time catalog id for VPN connections.
 * No parent wires.
 */
uint32_t
gj_nm_device_vpn_id_9433(void)
{
	(void)NULL;
	return b9433_vpn_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nm_device_vpn_id_9433(void)
    __attribute__((alias("gj_nm_device_vpn_id_9433")));
