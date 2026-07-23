/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14269: netmgr soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_netmgr_soft_ready_u_14269(void);
 *     - Returns 1 (soft lamp only). Indicates the netmgr soft stub
 *       surfaces in this continuum are present; not live network-manager
 *       readiness.
 *   uint32_t __gj_netmgr_soft_ready_u_14269  (alias)
 *   __libcgj_batch14269_marker = "libcgj-batch14269"
 *
 * Exclusive continuum CREATE-ONLY (14261-14270: netmgr soft stubs —
 * bluez_ok_u_14261, networkmanager_ok_u_14262, iwd_ok_u_14263,
 * wpa_supplicant_ok_u_14264, modemmanager_ok_u_14265,
 * connman_ok_u_14266, avahi_ok_u_14267, resolvconf_ok_u_14268,
 * netmgr_soft_ready_u_14269, batch_id_14270). Unique surface only; no
 * multi-def. Component ok units remain 0. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14269_marker[] = "libcgj-batch14269";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B14269_NETMGR_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14269_netmgr_soft_ready(void)
{
	return B14269_NETMGR_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_netmgr_soft_ready_u_14269 - netmgr soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 14261-14270 surfaces are present. Does not claim live BlueZ/
 * NetworkManager readiness and does not call libc. No parent wires.
 */
uint32_t
gj_netmgr_soft_ready_u_14269(void)
{
	(void)NULL;
	return b14269_netmgr_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_netmgr_soft_ready_u_14269(void)
    __attribute__((alias("gj_netmgr_soft_ready_u_14269")));
