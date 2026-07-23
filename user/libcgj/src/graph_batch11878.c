/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11878: resolvconf soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_resolvconf_ok_u_11878(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; resolvconf path is intentionally not reported ok.
 *   uint32_t __gj_resolvconf_ok_u_11878  (alias)
 *   __libcgj_batch11878_marker = "libcgj-batch11878"
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

const char __libcgj_batch11878_marker[] = "libcgj-batch11878";

/* Soft resolvconf-ok lamp: always off for this continuum. */
#define B11878_RESOLVCONF_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11878_resolvconf_ok(void)
{
	return B11878_RESOLVCONF_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_resolvconf_ok_u_11878 - resolvconf soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open resolvconf or call
 * libc. No parent wires.
 */
uint32_t
gj_resolvconf_ok_u_11878(void)
{
	(void)NULL;
	return b11878_resolvconf_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_resolvconf_ok_u_11878(void)
    __attribute__((alias("gj_resolvconf_ok_u_11878")));
