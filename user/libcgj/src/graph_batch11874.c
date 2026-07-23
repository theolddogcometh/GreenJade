/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11874: wpa_supplicant soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wpa_supplicant_ok_u_11874(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; wpa_supplicant path is intentionally not reported ok.
 *   uint32_t __gj_wpa_supplicant_ok_u_11874  (alias)
 *   __libcgj_batch11874_marker = "libcgj-batch11874"
 *
 * Exclusive continuum CREATE-ONLY (11871-11880: bluez/networkmanager
 * soft id stubs — bluez_ok_u_11871, networkmanager_ok_u_11872,
 * iwd_ok_u_11873, wpa_supplicant_ok_u_11874, modemmanager_ok_u_11875,
 * connman_ok_u_11876, avahi_ok_u_11877, resolvconf_ok_u_11878,
 * netmgr_soft_ready_u_11879, batch_id_11880). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11874_marker[] = "libcgj-batch11874";

/* Soft wpa_supplicant-ok lamp: always off for this continuum. */
#define B11874_WPA_SUPPLICANT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11874_wpa_supplicant_ok(void)
{
	return B11874_WPA_SUPPLICANT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wpa_supplicant_ok_u_11874 - wpa_supplicant soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open wpa_supplicant or call
 * libc. No parent wires.
 */
uint32_t
gj_wpa_supplicant_ok_u_11874(void)
{
	(void)NULL;
	return b11874_wpa_supplicant_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wpa_supplicant_ok_u_11874(void)
    __attribute__((alias("gj_wpa_supplicant_ok_u_11874")));
