/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12572: NetworkManager soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_networkmanager_ok_u_12572(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; NetworkManager path is intentionally not reported ok.
 *   uint32_t __gj_networkmanager_ok_u_12572  (alias)
 *   __libcgj_batch12572_marker = "libcgj-batch12572"
 *
 * Exclusive continuum CREATE-ONLY (12571-12580: bluez/networkmanager
 * soft id stubs — bluez_ok_u_12571, networkmanager_ok_u_12572,
 * iwd_ok_u_12573, wpa_supplicant_ok_u_12574, modemmanager_ok_u_12575,
 * connman_ok_u_12576, avahi_ok_u_12577, resolvconf_ok_u_12578,
 * netmgr_soft_ready_u_12579, batch_id_12580). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12572_marker[] = "libcgj-batch12572";

/* Soft NetworkManager-ok lamp: always off for this continuum. */
#define B12572_NETWORKMANAGER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12572_networkmanager_ok(void)
{
	return B12572_NETWORKMANAGER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_networkmanager_ok_u_12572 - NetworkManager soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open NetworkManager
 * or call libc. No parent wires.
 */
uint32_t
gj_networkmanager_ok_u_12572(void)
{
	(void)NULL;
	return b12572_networkmanager_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_networkmanager_ok_u_12572(void)
    __attribute__((alias("gj_networkmanager_ok_u_12572")));
