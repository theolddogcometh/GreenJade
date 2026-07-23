/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11077: Avahi soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_avahi_ok_u_11077(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; Avahi path is intentionally not reported ok.
 *   uint32_t __gj_avahi_ok_u_11077  (alias)
 *   __libcgj_batch11077_marker = "libcgj-batch11077"
 *
 * Exclusive continuum CREATE-ONLY (11071-11080: bluez/networkmanager
 * soft id stubs — bluez_ok_u_11071, networkmanager_ok_u_11072,
 * iwd_ok_u_11073, wpa_supplicant_ok_u_11074, modemmanager_ok_u_11075,
 * connman_ok_u_11076, avahi_ok_u_11077, resolvconf_ok_u_11078,
 * netmgr_soft_ready_u_11079, batch_id_11080). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11077_marker[] = "libcgj-batch11077";

/* Soft Avahi-ok lamp: always off for this continuum. */
#define B11077_AVAHI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11077_avahi_ok(void)
{
	return B11077_AVAHI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_avahi_ok_u_11077 - Avahi soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open Avahi or call
 * libc. No parent wires.
 */
uint32_t
gj_avahi_ok_u_11077(void)
{
	(void)NULL;
	return b11077_avahi_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_avahi_ok_u_11077(void)
    __attribute__((alias("gj_avahi_ok_u_11077")));
