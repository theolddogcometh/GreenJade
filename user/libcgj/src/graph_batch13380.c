/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13380: bluez/networkmanager soft id stubs
 * batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13380(void);
 *     - Returns the compile-time graph batch number for this TU (13380).
 *   uint32_t __gj_batch_id_13380  (alias)
 *   __libcgj_batch13380_marker = "libcgj-batch13380"
 *
 * Exclusive continuum CREATE-ONLY (13371-13380: bluez/networkmanager
 * soft id stubs — bluez_ok_u_13371, networkmanager_ok_u_13372,
 * iwd_ok_u_13373, wpa_supplicant_ok_u_13374, modemmanager_ok_u_13375,
 * connman_ok_u_13376, avahi_ok_u_13377, resolvconf_ok_u_13378,
 * netmgr_soft_ready_u_13379, batch_id_13380). Unique gj_batch_id_13380
 * surface only; no multi-def. Does NOT redefine gj_batch_id / prior
 * batch_id_* symbols - avoid multi-def. Closes the 13371-13380
 * continuum. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13380_marker[] = "libcgj-batch13380";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13380_id(void)
{
	return 13380u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13380 - report this TU's graph batch number.
 *
 * Always returns 13380. Soft pure-data identity for the
 * bluez/networkmanager soft id stubs continuum close. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_13380(void)
{
	(void)NULL;
	return b13380_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13380(void)
    __attribute__((alias("gj_batch_id_13380")));
