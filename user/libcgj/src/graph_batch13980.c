/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13980: bluez/networkmanager soft id stubs
 * batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13980(void);
 *     - Returns the compile-time graph batch number for this TU (13980).
 *   uint32_t __gj_batch_id_13980  (alias)
 *   __libcgj_batch13980_marker = "libcgj-batch13980"
 *
 * Exclusive continuum CREATE-ONLY (13971-13980: bluez/networkmanager
 * soft id stubs — bluez_ok_u_13971, networkmanager_ok_u_13972,
 * iwd_ok_u_13973, wpa_supplicant_ok_u_13974, modemmanager_ok_u_13975,
 * connman_ok_u_13976, avahi_ok_u_13977, resolvconf_ok_u_13978,
 * netmgr_soft_ready_u_13979, batch_id_13980). Unique gj_batch_id_13980
 * surface only; no multi-def. Does NOT redefine gj_batch_id / prior
 * batch_id_* symbols — avoid multi-def. Closes the 13971-13980
 * continuum. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13980_marker[] = "libcgj-batch13980";

#define B13980_BATCH_ID  13980u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13980_id(void)
{
	return B13980_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13980 - report this TU's graph batch number.
 *
 * Always returns 13980. Soft pure-data identity for the
 * bluez/networkmanager soft id stubs continuum close. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_13980(void)
{
	(void)NULL;
	return b13980_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13980(void)
    __attribute__((alias("gj_batch_id_13980")));
