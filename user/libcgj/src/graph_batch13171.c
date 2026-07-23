/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13171: BlueZ soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bluez_ok_u_13171(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; BlueZ path is intentionally not reported ok.
 *   uint32_t __gj_bluez_ok_u_13171  (alias)
 *   __libcgj_batch13171_marker = "libcgj-batch13171"
 *
 * Exclusive continuum CREATE-ONLY (13171-13180: bluez/networkmanager
 * soft id stubs — bluez_ok_u_13171, networkmanager_ok_u_13172,
 * iwd_ok_u_13173, wpa_supplicant_ok_u_13174, modemmanager_ok_u_13175,
 * connman_ok_u_13176, avahi_ok_u_13177, resolvconf_ok_u_13178,
 * netmgr_soft_ready_u_13179, batch_id_13180). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13171_marker[] = "libcgj-batch13171";

/* Soft BlueZ-ok lamp: always off for this continuum. */
#define B13171_BLUEZ_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13171_bluez_ok(void)
{
	return B13171_BLUEZ_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bluez_ok_u_13171 - BlueZ soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open BlueZ or call
 * libc. No parent wires.
 */
uint32_t
gj_bluez_ok_u_13171(void)
{
	(void)NULL;
	return b13171_bluez_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bluez_ok_u_13171(void)
    __attribute__((alias("gj_bluez_ok_u_13171")));
