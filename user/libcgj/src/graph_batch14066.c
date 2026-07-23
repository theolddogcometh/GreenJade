/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14066: ConnMan soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_connman_ok_u_14066(void);
 *     - Always returns 0. Soft stub for bluez/networkmanager soft-id
 *       continuum; ConnMan path is intentionally not reported ok.
 *   uint32_t __gj_connman_ok_u_14066  (alias)
 *   __libcgj_batch14066_marker = "libcgj-batch14066"
 *
 * Exclusive continuum CREATE-ONLY (14061-14070: bluez/networkmanager
 * soft id stubs — bluez_ok_u_14061, networkmanager_ok_u_14062,
 * iwd_ok_u_14063, wpa_supplicant_ok_u_14064, modemmanager_ok_u_14065,
 * connman_ok_u_14066, avahi_ok_u_14067, resolvconf_ok_u_14068,
 * netmgr_soft_ready_u_14069, batch_id_14070). Unique surface only; no
 * multi-def. Distinct from gj_connman_ok_u_13976 (batch13976) and
 * gj_connman_ok_u_13776 (batch13776). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14066_marker[] = "libcgj-batch14066";

/* Soft ConnMan-ok lamp: always off for this continuum. */
#define B14066_CONNMAN_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14066_connman_ok(void)
{
	return B14066_CONNMAN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_connman_ok_u_14066 - ConnMan soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open ConnMan or call
 * libc. No parent wires.
 */
uint32_t
gj_connman_ok_u_14066(void)
{
	(void)NULL;
	return b14066_connman_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_connman_ok_u_14066(void)
    __attribute__((alias("gj_connman_ok_u_14066")));
