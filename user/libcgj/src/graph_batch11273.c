/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11273: iwd soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_iwd_ok_u_11273(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; iwd path is intentionally not reported ok.
 *   uint32_t __gj_iwd_ok_u_11273  (alias)
 *   __libcgj_batch11273_marker = "libcgj-batch11273"
 *
 * Exclusive continuum CREATE-ONLY (11271-11280: bluez/networkmanager
 * soft id stubs — bluez_ok_u_11271, networkmanager_ok_u_11272,
 * iwd_ok_u_11273, wpa_supplicant_ok_u_11274, modemmanager_ok_u_11275,
 * connman_ok_u_11276, avahi_ok_u_11277, resolvconf_ok_u_11278,
 * netmgr_soft_ready_u_11279, batch_id_11280). Unique surface only; no
 * multi-def. Distinct from gj_iwd_ok_u_11073 (batch11073) and prior
 * netmgr soft waves. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11273_marker[] = "libcgj-batch11273";

/* Soft iwd-ok lamp: always off for this continuum. */
#define B11273_IWD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11273_iwd_ok(void)
{
	return B11273_IWD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iwd_ok_u_11273 - iwd soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open iwd or call
 * libc. No parent wires.
 */
uint32_t
gj_iwd_ok_u_11273(void)
{
	(void)NULL;
	return b11273_iwd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iwd_ok_u_11273(void)
    __attribute__((alias("gj_iwd_ok_u_11273")));
