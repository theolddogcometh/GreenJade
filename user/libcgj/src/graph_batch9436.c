/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9436: NetworkManager soft access-point ok tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nm_ap_ok_u_9436(void);
 *     - Return 1 (soft access-point / Wi-Fi AP surface accepted).
 *       Soft freestanding lamp; no D-Bus, no libnm, no scan.
 *   uint32_t __gj_nm_ap_ok_u_9436  (alias)
 *   __libcgj_batch9436_marker = "libcgj-batch9436"
 *
 * Exclusive continuum CREATE-ONLY networkmanager soft id stubs wave
 * (9431-9440: nm_device_wifi_id_9431, nm_device_eth_id_9432,
 * nm_device_vpn_id_9433, nm_state_connected_id_9434,
 * nm_state_disconnected_id_9435, nm_ap_ok_u_9436, nm_conn_ok_u_9437,
 * nm_settings_ok_u_9438, nm_ready_u_9439, batch_id_9440). Unique
 * gj_nm_ap_ok_u_9436 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9436_marker[] = "libcgj-batch9436";

/* Soft NM access-point surface acceptance lamp. */
#define B9436_NM_AP_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9436_ap_ok(void)
{
	return B9436_NM_AP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nm_ap_ok_u_9436 - soft NM access-point ok tag.
 *
 * Always returns 1 (AP surface soft-accepted). Does not scan SSIDs or
 * talk to NetworkManager. No parent wires.
 */
uint32_t
gj_nm_ap_ok_u_9436(void)
{
	(void)NULL;
	return b9436_ap_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nm_ap_ok_u_9436(void)
    __attribute__((alias("gj_nm_ap_ok_u_9436")));
