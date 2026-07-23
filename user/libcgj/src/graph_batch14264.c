/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14264: wpa_supplicant soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wpa_supplicant_ok_u_14264(void);
 *     - Always returns 0. Soft stub for netmgr soft continuum;
 *       wpa_supplicant path is intentionally not reported ok.
 *   uint32_t __gj_wpa_supplicant_ok_u_14264  (alias)
 *   __libcgj_batch14264_marker = "libcgj-batch14264"
 *
 * Exclusive continuum CREATE-ONLY (14261-14270: netmgr soft stubs —
 * bluez_ok_u_14261, networkmanager_ok_u_14262, iwd_ok_u_14263,
 * wpa_supplicant_ok_u_14264, modemmanager_ok_u_14265,
 * connman_ok_u_14266, avahi_ok_u_14267, resolvconf_ok_u_14268,
 * netmgr_soft_ready_u_14269, batch_id_14270). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14264_marker[] = "libcgj-batch14264";

/* Soft wpa_supplicant-ok lamp: always off for this continuum. */
#define B14264_WPA_SUPPLICANT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14264_wpa_supplicant_ok(void)
{
	return B14264_WPA_SUPPLICANT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wpa_supplicant_ok_u_14264 - wpa_supplicant soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open wpa_supplicant
 * or call libc. No parent wires.
 */
uint32_t
gj_wpa_supplicant_ok_u_14264(void)
{
	(void)NULL;
	return b14264_wpa_supplicant_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wpa_supplicant_ok_u_14264(void)
    __attribute__((alias("gj_wpa_supplicant_ok_u_14264")));
