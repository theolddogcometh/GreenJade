/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13375: ModemManager soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_modemmanager_ok_u_13375(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; ModemManager path is intentionally not reported ok.
 *   uint32_t __gj_modemmanager_ok_u_13375  (alias)
 *   __libcgj_batch13375_marker = "libcgj-batch13375"
 *
 * Exclusive continuum CREATE-ONLY (13371-13380: bluez/networkmanager
 * soft id stubs — bluez_ok_u_13371, networkmanager_ok_u_13372,
 * iwd_ok_u_13373, wpa_supplicant_ok_u_13374, modemmanager_ok_u_13375,
 * connman_ok_u_13376, avahi_ok_u_13377, resolvconf_ok_u_13378,
 * netmgr_soft_ready_u_13379, batch_id_13380). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13375_marker[] = "libcgj-batch13375";

/* Soft ModemManager-ok lamp: always off for this continuum. */
#define B13375_MODEMMANAGER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13375_modemmanager_ok(void)
{
	return B13375_MODEMMANAGER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_modemmanager_ok_u_13375 - ModemManager soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open ModemManager or
 * call libc. No parent wires.
 */
uint32_t
gj_modemmanager_ok_u_13375(void)
{
	(void)NULL;
	return b13375_modemmanager_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_modemmanager_ok_u_13375(void)
    __attribute__((alias("gj_modemmanager_ok_u_13375")));
