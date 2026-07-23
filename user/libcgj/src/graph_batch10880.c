/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10880: bluez/networkmanager soft id stubs
 * batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10880(void);
 *     - Returns the compile-time graph batch number for this TU (10880).
 *   uint32_t __gj_batch_id_10880  (alias)
 *   __libcgj_batch10880_marker = "libcgj-batch10880"
 *
 * Exclusive continuum CREATE-ONLY (10871-10880: bluez/networkmanager
 * soft id stubs — bluez_ok_u_10871, networkmanager_ok_u_10872,
 * iwd_ok_u_10873, wpa_supplicant_ok_u_10874, modemmanager_ok_u_10875,
 * connman_ok_u_10876, avahi_ok_u_10877, resolvconf_ok_u_10878,
 * netmgr_soft_ready_u_10879, batch_id_10880). Unique gj_batch_id_10880
 * surface only; no multi-def. Does NOT redefine gj_batch_id / prior
 * batch_id_* symbols - avoid multi-def. Closes the 10871-10880
 * continuum. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10880_marker[] = "libcgj-batch10880";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10880_id(void)
{
	return 10880u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10880 - report this TU's graph batch number.
 *
 * Always returns 10880. Soft pure-data identity for the
 * bluez/networkmanager soft id stubs continuum close. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_10880(void)
{
	(void)NULL;
	return b10880_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10880(void)
    __attribute__((alias("gj_batch_id_10880")));
