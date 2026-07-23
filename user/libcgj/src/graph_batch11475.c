/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11475: ModemManager soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_modemmanager_ok_u_11475(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; ModemManager path is intentionally not reported ok.
 *   uint32_t __gj_modemmanager_ok_u_11475  (alias)
 *   __libcgj_batch11475_marker = "libcgj-batch11475"
 *
 * Exclusive continuum CREATE-ONLY (11471-11480: bluez/networkmanager
 * soft id stubs — bluez_ok_u_11471, networkmanager_ok_u_11472,
 * iwd_ok_u_11473, wpa_supplicant_ok_u_11474, modemmanager_ok_u_11475,
 * connman_ok_u_11476, avahi_ok_u_11477, resolvconf_ok_u_11478,
 * netmgr_soft_ready_u_11479, batch_id_11480). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11475_marker[] = "libcgj-batch11475";

/* Soft ModemManager-ok lamp: always off for this continuum. */
#define B11475_MODEMMANAGER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11475_modemmanager_ok(void)
{
	return B11475_MODEMMANAGER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_modemmanager_ok_u_11475 - ModemManager soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open ModemManager or
 * call libc. No parent wires.
 */
uint32_t
gj_modemmanager_ok_u_11475(void)
{
	(void)NULL;
	return b11475_modemmanager_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_modemmanager_ok_u_11475(void)
    __attribute__((alias("gj_modemmanager_ok_u_11475")));
