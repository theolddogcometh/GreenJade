/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12976: ConnMan soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_connman_ok_u_12976(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; ConnMan path is intentionally not reported ok.
 *   uint32_t __gj_connman_ok_u_12976  (alias)
 *   __libcgj_batch12976_marker = "libcgj-batch12976"
 *
 * Exclusive continuum CREATE-ONLY (12971-12980: bluez/networkmanager
 * soft id stubs — bluez_ok_u_12971, networkmanager_ok_u_12972,
 * iwd_ok_u_12973, wpa_supplicant_ok_u_12974, modemmanager_ok_u_12975,
 * connman_ok_u_12976, avahi_ok_u_12977, resolvconf_ok_u_12978,
 * netmgr_soft_ready_u_12979, batch_id_12980). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12976_marker[] = "libcgj-batch12976";

/* Soft ConnMan-ok lamp: always off for this continuum. */
#define B12976_CONNMAN_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12976_connman_ok(void)
{
	return B12976_CONNMAN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_connman_ok_u_12976 - ConnMan soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open ConnMan or call
 * libc. No parent wires.
 */
uint32_t
gj_connman_ok_u_12976(void)
{
	(void)NULL;
	return b12976_connman_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_connman_ok_u_12976(void)
    __attribute__((alias("gj_connman_ok_u_12976")));
