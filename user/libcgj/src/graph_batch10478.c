/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10478: resolvconf ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_resolvconf_ok_u_10478(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       resolvconf/resolv.conf runtime probe for the netmgr soft path.
 *   uint32_t __gj_resolvconf_ok_u_10478  (alias)
 *   __libcgj_batch10478_marker = "libcgj-batch10478"
 *
 * Exclusive continuum CREATE-ONLY (10471-10480: netmgr soft id stubs —
 * bluez_ok_u_10471, networkmanager_ok_u_10472, iwd_ok_u_10473,
 * wpa_supplicant_ok_u_10474, modemmanager_ok_u_10475,
 * connman_ok_u_10476, avahi_ok_u_10477, resolvconf_ok_u_10478,
 * netmgr_soft_ready_u_10479, batch_id_10480). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10478_marker[] = "libcgj-batch10478";

/* Soft resolvconf-ok lamp: always off (not a real runtime probe). */
#define B10478_RESOLVCONF_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10478_ok(void)
{
	return B10478_RESOLVCONF_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_resolvconf_ok_u_10478 - resolvconf ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe resolvconf or
 * resolv.conf presence or call libc. No parent wires.
 */
uint32_t
gj_resolvconf_ok_u_10478(void)
{
	(void)NULL;
	return b10478_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_resolvconf_ok_u_10478(void)
    __attribute__((alias("gj_resolvconf_ok_u_10478")));
