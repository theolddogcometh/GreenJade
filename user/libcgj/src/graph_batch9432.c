/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9432: NetworkManager soft ETHERNET device type id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nm_device_eth_id_9432(void);
 *     - Return the classic NM device-type code for ETHERNET (1).
 *       Soft freestanding stub constant; no D-Bus, no libnm.
 *   uint32_t __gj_nm_device_eth_id_9432  (alias)
 *   __libcgj_batch9432_marker = "libcgj-batch9432"
 *
 * Exclusive continuum CREATE-ONLY networkmanager soft id stubs wave
 * (9431-9440: nm_device_wifi_id_9431, nm_device_eth_id_9432,
 * nm_device_vpn_id_9433, nm_state_connected_id_9434,
 * nm_state_disconnected_id_9435, nm_ap_ok_u_9436, nm_conn_ok_u_9437,
 * nm_settings_ok_u_9438, nm_ready_u_9439, batch_id_9440). Unique
 * gj_nm_device_eth_id_9432 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9432_marker[] = "libcgj-batch9432";

/* NM_DEVICE_TYPE_ETHERNET (classic NetworkManager device-type ladder). */
#define B9432_NM_DEVICE_TYPE_ETHERNET  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9432_eth_id(void)
{
	return B9432_NM_DEVICE_TYPE_ETHERNET;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nm_device_eth_id_9432 - report classic ETHERNET device type id.
 *
 * Always returns 1. Soft compile-time catalog id for NM ethernet devices.
 * No parent wires.
 */
uint32_t
gj_nm_device_eth_id_9432(void)
{
	(void)NULL;
	return b9432_eth_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nm_device_eth_id_9432(void)
    __attribute__((alias("gj_nm_device_eth_id_9432")));
