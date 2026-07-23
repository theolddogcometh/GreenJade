/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11675: ModemManager soft-ok unit (off)
 *
 * Surface (unique symbols):
 *   uint32_t gj_modemmanager_ok_u_11675(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; ModemManager path is intentionally not reported ok.
 *   uint32_t __gj_modemmanager_ok_u_11675  (alias)
 *   __libcgj_batch11675_marker = "libcgj-batch11675"
 *
 * Exclusive continuum CREATE-ONLY (11671-11680: bluez/networkmanager
 * soft id stubs — bluez_ok_u_11671, networkmanager_ok_u_11672,
 * iwd_ok_u_11673, wpa_supplicant_ok_u_11674, modemmanager_ok_u_11675,
 * connman_ok_u_11676, avahi_ok_u_11677, resolvconf_ok_u_11678,
 * netmgr_soft_ready_u_11679, batch_id_11680). Unique surface only; no
 * multi-def. Distinct from gj_modemmanager_ok_u_11475 (batch11475), gj_modemmanager_ok_u_11275 (batch11275).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11675_marker[] = "libcgj-batch11675";

/* Soft ModemManager-ok lamp: always off for this continuum. */
#define B11675_MODEMMANAGER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11675_modemmanager_ok(void)
{
	return B11675_MODEMMANAGER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_modemmanager_ok_u_11675 - ModemManager soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open ModemManager or
 * call libc. No parent wires.
 */
uint32_t
gj_modemmanager_ok_u_11675(void)
{
	(void)NULL;
	return b11675_modemmanager_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_modemmanager_ok_u_11675(void)
    __attribute__((alias("gj_modemmanager_ok_u_11675")));
