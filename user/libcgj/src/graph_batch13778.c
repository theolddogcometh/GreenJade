/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13778: resolvconf soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_resolvconf_ok_u_13778(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; resolvconf path is intentionally not reported ok.
 *   uint32_t __gj_resolvconf_ok_u_13778  (alias)
 *   __libcgj_batch13778_marker = "libcgj-batch13778"
 *
 * Exclusive continuum CREATE-ONLY (13771-13780: bluez/networkmanager
 * soft id stubs — bluez_ok_u_13771, networkmanager_ok_u_13772,
 * iwd_ok_u_13773, wpa_supplicant_ok_u_13774, modemmanager_ok_u_13775,
 * connman_ok_u_13776, avahi_ok_u_13777, resolvconf_ok_u_13778,
 * netmgr_soft_ready_u_13779, batch_id_13780). Unique surface only; no
 * multi-def. Distinct from gj_resolvconf_ok_u_10178. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13778_marker[] = "libcgj-batch13778";

/* Soft resolvconf-ok lamp: always off for this continuum. */
#define B13778_RESOLVCONF_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13778_resolvconf_ok(void)
{
	return B13778_RESOLVCONF_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_resolvconf_ok_u_13778 - resolvconf soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not touch resolvconf or
 * call libc. No parent wires.
 */
uint32_t
gj_resolvconf_ok_u_13778(void)
{
	(void)NULL;
	return b13778_resolvconf_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_resolvconf_ok_u_13778(void)
    __attribute__((alias("gj_resolvconf_ok_u_13778")));
