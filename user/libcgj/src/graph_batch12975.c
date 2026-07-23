/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12975: ModemManager soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_modemmanager_ok_u_12975(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; ModemManager path is intentionally not reported ok.
 *   uint32_t __gj_modemmanager_ok_u_12975  (alias)
 *   __libcgj_batch12975_marker = "libcgj-batch12975"
 *
 * Exclusive continuum CREATE-ONLY (12971-12980: bluez/networkmanager
 * soft id stubs — bluez_ok_u_12971, networkmanager_ok_u_12972,
 * iwd_ok_u_12973, wpa_supplicant_ok_u_12974, modemmanager_ok_u_12975,
 * connman_ok_u_12976, avahi_ok_u_12977, resolvconf_ok_u_12978,
 * netmgr_soft_ready_u_12979, batch_id_12980). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12975_marker[] = "libcgj-batch12975";

/* Soft ModemManager-ok lamp: always off for this continuum. */
#define B12975_MODEMMANAGER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12975_modemmanager_ok(void)
{
	return B12975_MODEMMANAGER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_modemmanager_ok_u_12975 - ModemManager soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open ModemManager or
 * call libc. No parent wires.
 */
uint32_t
gj_modemmanager_ok_u_12975(void)
{
	(void)NULL;
	return b12975_modemmanager_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_modemmanager_ok_u_12975(void)
    __attribute__((alias("gj_modemmanager_ok_u_12975")));
