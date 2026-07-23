/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13974: wpa_supplicant soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wpa_supplicant_ok_u_13974(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; wpa_supplicant path is intentionally not reported ok.
 *   uint32_t __gj_wpa_supplicant_ok_u_13974  (alias)
 *   __libcgj_batch13974_marker = "libcgj-batch13974"
 *
 * Exclusive continuum CREATE-ONLY (13971-13980: bluez/networkmanager
 * soft id stubs — bluez_ok_u_13971, networkmanager_ok_u_13972,
 * iwd_ok_u_13973, wpa_supplicant_ok_u_13974, modemmanager_ok_u_13975,
 * connman_ok_u_13976, avahi_ok_u_13977, resolvconf_ok_u_13978,
 * netmgr_soft_ready_u_13979, batch_id_13980). Unique surface only; no
 * multi-def. Distinct from gj_wpa_supplicant_ok_u_13774 (batch13774)
 * and gj_wpa_supplicant_ok_u_10174 (batch10174). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13974_marker[] = "libcgj-batch13974";

/* Soft wpa_supplicant-ok lamp: always off for this continuum. */
#define B13974_WPA_SUPPLICANT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13974_wpa_supplicant_ok(void)
{
	return B13974_WPA_SUPPLICANT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wpa_supplicant_ok_u_13974 - wpa_supplicant soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open wpa_supplicant
 * or call libc. No parent wires.
 */
uint32_t
gj_wpa_supplicant_ok_u_13974(void)
{
	(void)NULL;
	return b13974_wpa_supplicant_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wpa_supplicant_ok_u_13974(void)
    __attribute__((alias("gj_wpa_supplicant_ok_u_13974")));
