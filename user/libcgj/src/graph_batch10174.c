/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10174: wpa_supplicant soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wpa_supplicant_ok_u_10174(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; wpa_supplicant path is intentionally not reported ok.
 *   uint32_t __gj_wpa_supplicant_ok_u_10174  (alias)
 *   __libcgj_batch10174_marker = "libcgj-batch10174"
 *
 * Exclusive continuum CREATE-ONLY (10171-10180: bluez/networkmanager
 * soft id stubs — bluez_ok_u_10171, networkmanager_ok_u_10172,
 * iwd_ok_u_10173, wpa_supplicant_ok_u_10174, modemmanager_ok_u_10175,
 * connman_ok_u_10176, avahi_ok_u_10177, resolvconf_ok_u_10178,
 * netmgr_soft_ready_u_10179, batch_id_10180). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10174_marker[] = "libcgj-batch10174";

/* Soft wpa_supplicant-ok lamp: always off for this continuum. */
#define B10174_WPA_SUPPLICANT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10174_wpa_supplicant_ok(void)
{
	return B10174_WPA_SUPPLICANT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wpa_supplicant_ok_u_10174 - wpa_supplicant soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open wpa_supplicant
 * or call libc. No parent wires.
 */
uint32_t
gj_wpa_supplicant_ok_u_10174(void)
{
	(void)NULL;
	return b10174_wpa_supplicant_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wpa_supplicant_ok_u_10174(void)
    __attribute__((alias("gj_wpa_supplicant_ok_u_10174")));
