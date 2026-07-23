/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13575: ModemManager soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_modemmanager_ok_u_13575(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; ModemManager path is intentionally not reported ok.
 *   uint32_t __gj_modemmanager_ok_u_13575  (alias)
 *   __libcgj_batch13575_marker = "libcgj-batch13575"
 *
 * Exclusive continuum CREATE-ONLY (13571-13580: bluez/networkmanager
 * soft id stubs — bluez_ok_u_13571, networkmanager_ok_u_13572,
 * iwd_ok_u_13573, wpa_supplicant_ok_u_13574, modemmanager_ok_u_13575,
 * connman_ok_u_13576, avahi_ok_u_13577, resolvconf_ok_u_13578,
 * netmgr_soft_ready_u_13579, batch_id_13580). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13575_marker[] = "libcgj-batch13575";

/* Soft ModemManager-ok lamp: always off for this continuum. */
#define B13575_MODEMMANAGER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13575_modemmanager_ok(void)
{
	return B13575_MODEMMANAGER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_modemmanager_ok_u_13575 - ModemManager soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open ModemManager or
 * call libc. No parent wires.
 */
uint32_t
gj_modemmanager_ok_u_13575(void)
{
	(void)NULL;
	return b13575_modemmanager_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_modemmanager_ok_u_13575(void)
    __attribute__((alias("gj_modemmanager_ok_u_13575")));
