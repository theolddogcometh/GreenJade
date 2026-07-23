/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12772: NetworkManager ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_networkmanager_ok_u_12772(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       NetworkManager runtime probe for the netmgr soft path.
 *   uint32_t __gj_networkmanager_ok_u_12772  (alias)
 *   __libcgj_batch12772_marker = "libcgj-batch12772"
 *
 * Exclusive continuum CREATE-ONLY (12771-12780: netmgr soft id stubs —
 * bluez_ok_u_12771, networkmanager_ok_u_12772, iwd_ok_u_12773,
 * wpa_supplicant_ok_u_12774, modemmanager_ok_u_12775,
 * connman_ok_u_12776, avahi_ok_u_12777, resolvconf_ok_u_12778,
 * netmgr_soft_ready_u_12779, batch_id_12780). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12772_marker[] = "libcgj-batch12772";

/* Soft NetworkManager-ok lamp: always off (not a real runtime probe). */
#define B12772_NETWORKMANAGER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12772_ok(void)
{
	return B12772_NETWORKMANAGER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_networkmanager_ok_u_12772 - NetworkManager ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe NetworkManager
 * presence or call libc. No parent wires.
 */
uint32_t
gj_networkmanager_ok_u_12772(void)
{
	(void)NULL;
	return b12772_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_networkmanager_ok_u_12772(void)
    __attribute__((alias("gj_networkmanager_ok_u_12772")));
