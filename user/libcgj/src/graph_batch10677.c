/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10677: Avahi ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_avahi_ok_u_10677(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Avahi mDNS/DNS-SD runtime probe for the netmgr soft path.
 *   uint32_t __gj_avahi_ok_u_10677  (alias)
 *   __libcgj_batch10677_marker = "libcgj-batch10677"
 *
 * Exclusive continuum CREATE-ONLY (10671-10680: netmgr soft id stubs —
 * bluez_ok_u_10671, networkmanager_ok_u_10672, iwd_ok_u_10673,
 * wpa_supplicant_ok_u_10674, modemmanager_ok_u_10675,
 * connman_ok_u_10676, avahi_ok_u_10677, resolvconf_ok_u_10678,
 * netmgr_soft_ready_u_10679, batch_id_10680). Unique surface only;
 * no multi-def. Distinct from gj_avahi_ok_u_10477 (batch10477).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10677_marker[] = "libcgj-batch10677";

/* Soft Avahi-ok lamp: always off (not a real runtime probe). */
#define B10677_AVAHI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10677_ok(void)
{
	return B10677_AVAHI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_avahi_ok_u_10677 - Avahi ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Avahi daemon
 * presence or call libc. No parent wires.
 */
uint32_t
gj_avahi_ok_u_10677(void)
{
	(void)NULL;
	return b10677_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_avahi_ok_u_10677(void)
    __attribute__((alias("gj_avahi_ok_u_10677")));
