/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10172: NetworkManager soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_networkmanager_ok_u_10172(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; NetworkManager path is intentionally not reported ok.
 *   uint32_t __gj_networkmanager_ok_u_10172  (alias)
 *   __libcgj_batch10172_marker = "libcgj-batch10172"
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

const char __libcgj_batch10172_marker[] = "libcgj-batch10172";

/* Soft NetworkManager-ok lamp: always off for this continuum. */
#define B10172_NETWORKMANAGER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10172_networkmanager_ok(void)
{
	return B10172_NETWORKMANAGER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_networkmanager_ok_u_10172 - NetworkManager soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open NetworkManager
 * or call libc. No parent wires.
 */
uint32_t
gj_networkmanager_ok_u_10172(void)
{
	(void)NULL;
	return b10172_networkmanager_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_networkmanager_ok_u_10172(void)
    __attribute__((alias("gj_networkmanager_ok_u_10172")));
