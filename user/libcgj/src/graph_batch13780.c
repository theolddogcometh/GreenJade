/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13780: bluez/networkmanager soft id stubs
 * batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13780(void);
 *     - Returns the compile-time graph batch number for this TU (13780).
 *   uint32_t __gj_batch_id_13780  (alias)
 *   __libcgj_batch13780_marker = "libcgj-batch13780"
 *
 * Exclusive continuum CREATE-ONLY (13771-13780: bluez/networkmanager
 * soft id stubs — bluez_ok_u_13771, networkmanager_ok_u_13772,
 * iwd_ok_u_13773, wpa_supplicant_ok_u_13774, modemmanager_ok_u_13775,
 * connman_ok_u_13776, avahi_ok_u_13777, resolvconf_ok_u_13778,
 * netmgr_soft_ready_u_13779, batch_id_13780). Unique gj_batch_id_13780
 * surface only; no multi-def. Does NOT redefine gj_batch_id / prior
 * batch_id_* symbols — avoid multi-def. Closes the 13771-13780
 * continuum. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13780_marker[] = "libcgj-batch13780";

#define B13780_BATCH_ID  13780u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13780_id(void)
{
	return B13780_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13780 - report this TU's graph batch number.
 *
 * Always returns 13780. Soft pure-data identity for the
 * bluez/networkmanager soft id stubs continuum close. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_13780(void)
{
	(void)NULL;
	return b13780_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13780(void)
    __attribute__((alias("gj_batch_id_13780")));
