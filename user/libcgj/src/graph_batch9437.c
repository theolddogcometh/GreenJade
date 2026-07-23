/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9437: NetworkManager soft connection ok tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nm_conn_ok_u_9437(void);
 *     - Return 1 (soft connection / active-connection surface accepted).
 *       Soft freestanding lamp; no D-Bus, no libnm.
 *   uint32_t __gj_nm_conn_ok_u_9437  (alias)
 *   __libcgj_batch9437_marker = "libcgj-batch9437"
 *
 * Exclusive continuum CREATE-ONLY networkmanager soft id stubs wave
 * (9431-9440: nm_device_wifi_id_9431, nm_device_eth_id_9432,
 * nm_device_vpn_id_9433, nm_state_connected_id_9434,
 * nm_state_disconnected_id_9435, nm_ap_ok_u_9436, nm_conn_ok_u_9437,
 * nm_settings_ok_u_9438, nm_ready_u_9439, batch_id_9440). Unique
 * gj_nm_conn_ok_u_9437 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9437_marker[] = "libcgj-batch9437";

/* Soft NM connection surface acceptance lamp. */
#define B9437_NM_CONN_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9437_conn_ok(void)
{
	return B9437_NM_CONN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nm_conn_ok_u_9437 - soft NM connection ok tag.
 *
 * Always returns 1 (connection surface soft-accepted). Does not activate
 * connections or talk to NetworkManager. No parent wires.
 */
uint32_t
gj_nm_conn_ok_u_9437(void)
{
	(void)NULL;
	return b9437_conn_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nm_conn_ok_u_9437(void)
    __attribute__((alias("gj_nm_conn_ok_u_9437")));
