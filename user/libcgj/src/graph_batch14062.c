/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14062: NetworkManager soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_networkmanager_ok_u_14062(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; NetworkManager path is intentionally not reported ok.
 *   uint32_t __gj_networkmanager_ok_u_14062  (alias)
 *   __libcgj_batch14062_marker = "libcgj-batch14062"
 *
 * Exclusive continuum CREATE-ONLY (14061-14070: bluez/networkmanager
 * soft id stubs — bluez_ok_u_14061, networkmanager_ok_u_14062,
 * iwd_ok_u_14063, wpa_supplicant_ok_u_14064, modemmanager_ok_u_14065,
 * connman_ok_u_14066, avahi_ok_u_14067, resolvconf_ok_u_14068,
 * netmgr_soft_ready_u_14069, batch_id_14070). Unique surface only; no
 * multi-def. Distinct from gj_networkmanager_ok_u_13972 (batch13972)
 * and gj_networkmanager_ok_u_13772 (batch13772). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14062_marker[] = "libcgj-batch14062";

/* Soft NetworkManager-ok lamp: always off for this continuum. */
#define B14062_NETWORKMANAGER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14062_networkmanager_ok(void)
{
	return B14062_NETWORKMANAGER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_networkmanager_ok_u_14062 - NetworkManager soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open NetworkManager
 * or call libc. No parent wires.
 */
uint32_t
gj_networkmanager_ok_u_14062(void)
{
	(void)NULL;
	return b14062_networkmanager_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_networkmanager_ok_u_14062(void)
    __attribute__((alias("gj_networkmanager_ok_u_14062")));
