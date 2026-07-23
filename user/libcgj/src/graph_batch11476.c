/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11476: ConnMan soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_connman_ok_u_11476(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; ConnMan path is intentionally not reported ok.
 *   uint32_t __gj_connman_ok_u_11476  (alias)
 *   __libcgj_batch11476_marker = "libcgj-batch11476"
 *
 * Exclusive continuum CREATE-ONLY (11471-11480: bluez/networkmanager
 * soft id stubs — bluez_ok_u_11471, networkmanager_ok_u_11472,
 * iwd_ok_u_11473, wpa_supplicant_ok_u_11474, modemmanager_ok_u_11475,
 * connman_ok_u_11476, avahi_ok_u_11477, resolvconf_ok_u_11478,
 * netmgr_soft_ready_u_11479, batch_id_11480). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11476_marker[] = "libcgj-batch11476";

/* Soft ConnMan-ok lamp: always off for this continuum. */
#define B11476_CONNMAN_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11476_connman_ok(void)
{
	return B11476_CONNMAN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_connman_ok_u_11476 - ConnMan soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open ConnMan or call
 * libc. No parent wires.
 */
uint32_t
gj_connman_ok_u_11476(void)
{
	(void)NULL;
	return b11476_connman_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_connman_ok_u_11476(void)
    __attribute__((alias("gj_connman_ok_u_11476")));
