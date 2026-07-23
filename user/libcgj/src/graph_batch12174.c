/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12174: wpa_supplicant soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wpa_supplicant_ok_u_12174(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; wpa_supplicant path is intentionally not reported ok.
 *   uint32_t __gj_wpa_supplicant_ok_u_12174  (alias)
 *   __libcgj_batch12174_marker = "libcgj-batch12174"
 *
 * Exclusive continuum CREATE-ONLY (12171-12180: bluez/networkmanager
 * soft id stubs — bluez_ok_u_12171, networkmanager_ok_u_12172,
 * iwd_ok_u_12173, wpa_supplicant_ok_u_12174, modemmanager_ok_u_12175,
 * connman_ok_u_12176, avahi_ok_u_12177, resolvconf_ok_u_12178,
 * netmgr_soft_ready_u_12179, batch_id_12180). Unique surface only; no
 * multi-def. Distinct from gj_wpa_supplicant_ok_u_11874 (batch11874),
 * gj_wpa_supplicant_ok_u_11674 (batch11674). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12174_marker[] = "libcgj-batch12174";

/* Soft wpa_supplicant-ok lamp: always off for this continuum. */
#define B12174_WPA_SUPPLICANT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12174_wpa_supplicant_ok(void)
{
	return B12174_WPA_SUPPLICANT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wpa_supplicant_ok_u_12174 - wpa_supplicant soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open wpa_supplicant
 * or call libc. No parent wires.
 */
uint32_t
gj_wpa_supplicant_ok_u_12174(void)
{
	(void)NULL;
	return b12174_wpa_supplicant_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wpa_supplicant_ok_u_12174(void)
    __attribute__((alias("gj_wpa_supplicant_ok_u_12174")));
