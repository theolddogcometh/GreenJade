/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11278: resolvconf soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_resolvconf_ok_u_11278(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; resolvconf path is intentionally not reported ok.
 *   uint32_t __gj_resolvconf_ok_u_11278  (alias)
 *   __libcgj_batch11278_marker = "libcgj-batch11278"
 *
 * Exclusive continuum CREATE-ONLY (11271-11280: bluez/networkmanager
 * soft id stubs — bluez_ok_u_11271, networkmanager_ok_u_11272,
 * iwd_ok_u_11273, wpa_supplicant_ok_u_11274, modemmanager_ok_u_11275,
 * connman_ok_u_11276, avahi_ok_u_11277, resolvconf_ok_u_11278,
 * netmgr_soft_ready_u_11279, batch_id_11280). Unique surface only; no
 * multi-def. Distinct from gj_resolvconf_ok_u_11078 (batch11078) and
 * prior netmgr soft waves. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11278_marker[] = "libcgj-batch11278";

/* Soft resolvconf-ok lamp: always off for this continuum. */
#define B11278_RESOLVCONF_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11278_resolvconf_ok(void)
{
	return B11278_RESOLVCONF_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_resolvconf_ok_u_11278 - resolvconf soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not touch resolvconf or
 * call libc. No parent wires.
 */
uint32_t
gj_resolvconf_ok_u_11278(void)
{
	(void)NULL;
	return b11278_resolvconf_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_resolvconf_ok_u_11278(void)
    __attribute__((alias("gj_resolvconf_ok_u_11278")));
