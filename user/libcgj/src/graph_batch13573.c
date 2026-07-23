/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13573: iwd soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_iwd_ok_u_13573(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; iwd path is intentionally not reported ok.
 *   uint32_t __gj_iwd_ok_u_13573  (alias)
 *   __libcgj_batch13573_marker = "libcgj-batch13573"
 *
 * Exclusive continuum CREATE-ONLY (13571-13580: bluez/networkmanager
 * soft id stubs — bluez_ok_u_13571, networkmanager_ok_u_13572,
 * iwd_ok_u_13573, wpa_supplicant_ok_u_13574, modemmanager_ok_u_13575,
 * connman_ok_u_13576, avahi_ok_u_13577, resolvconf_ok_u_13578,
 * netmgr_soft_ready_u_13579, batch_id_13580). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13573_marker[] = "libcgj-batch13573";

/* Soft iwd-ok lamp: always off for this continuum. */
#define B13573_IWD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13573_iwd_ok(void)
{
	return B13573_IWD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iwd_ok_u_13573 - iwd soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open iwd or call
 * libc. No parent wires.
 */
uint32_t
gj_iwd_ok_u_13573(void)
{
	(void)NULL;
	return b13573_iwd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iwd_ok_u_13573(void)
    __attribute__((alias("gj_iwd_ok_u_13573")));
