/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11873: iwd soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_iwd_ok_u_11873(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; iwd path is intentionally not reported ok.
 *   uint32_t __gj_iwd_ok_u_11873  (alias)
 *   __libcgj_batch11873_marker = "libcgj-batch11873"
 *
 * Exclusive continuum CREATE-ONLY (11871-11880: bluez/networkmanager
 * soft id stubs — bluez_ok_u_11871, networkmanager_ok_u_11872,
 * iwd_ok_u_11873, wpa_supplicant_ok_u_11874, modemmanager_ok_u_11875,
 * connman_ok_u_11876, avahi_ok_u_11877, resolvconf_ok_u_11878,
 * netmgr_soft_ready_u_11879, batch_id_11880). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11873_marker[] = "libcgj-batch11873";

/* Soft iwd-ok lamp: always off for this continuum. */
#define B11873_IWD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11873_iwd_ok(void)
{
	return B11873_IWD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iwd_ok_u_11873 - iwd soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open iwd or call
 * libc. No parent wires.
 */
uint32_t
gj_iwd_ok_u_11873(void)
{
	(void)NULL;
	return b11873_iwd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iwd_ok_u_11873(void)
    __attribute__((alias("gj_iwd_ok_u_11873")));
