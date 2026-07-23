/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11677: Avahi soft-ok unit (off)
 *
 * Surface (unique symbols):
 *   uint32_t gj_avahi_ok_u_11677(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; Avahi path is intentionally not reported ok.
 *   uint32_t __gj_avahi_ok_u_11677  (alias)
 *   __libcgj_batch11677_marker = "libcgj-batch11677"
 *
 * Exclusive continuum CREATE-ONLY (11671-11680: bluez/networkmanager
 * soft id stubs — bluez_ok_u_11671, networkmanager_ok_u_11672,
 * iwd_ok_u_11673, wpa_supplicant_ok_u_11674, modemmanager_ok_u_11675,
 * connman_ok_u_11676, avahi_ok_u_11677, resolvconf_ok_u_11678,
 * netmgr_soft_ready_u_11679, batch_id_11680). Unique surface only; no
 * multi-def. Distinct from gj_avahi_ok_u_11477 (batch11477), gj_avahi_ok_u_11277 (batch11277).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11677_marker[] = "libcgj-batch11677";

/* Soft Avahi-ok lamp: always off for this continuum. */
#define B11677_AVAHI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11677_avahi_ok(void)
{
	return B11677_AVAHI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_avahi_ok_u_11677 - Avahi soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open Avahi or call
 * libc. No parent wires.
 */
uint32_t
gj_avahi_ok_u_11677(void)
{
	(void)NULL;
	return b11677_avahi_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_avahi_ok_u_11677(void)
    __attribute__((alias("gj_avahi_ok_u_11677")));
