/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10676: ConnMan ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_connman_ok_u_10676(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       ConnMan runtime probe for the netmgr soft path.
 *   uint32_t __gj_connman_ok_u_10676  (alias)
 *   __libcgj_batch10676_marker = "libcgj-batch10676"
 *
 * Exclusive continuum CREATE-ONLY (10671-10680: netmgr soft id stubs —
 * bluez_ok_u_10671, networkmanager_ok_u_10672, iwd_ok_u_10673,
 * wpa_supplicant_ok_u_10674, modemmanager_ok_u_10675,
 * connman_ok_u_10676, avahi_ok_u_10677, resolvconf_ok_u_10678,
 * netmgr_soft_ready_u_10679, batch_id_10680). Unique surface only;
 * no multi-def. Distinct from gj_connman_ok_u_10476 (batch10476).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10676_marker[] = "libcgj-batch10676";

/* Soft ConnMan-ok lamp: always off (not a real runtime probe). */
#define B10676_CONNMAN_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10676_ok(void)
{
	return B10676_CONNMAN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_connman_ok_u_10676 - ConnMan ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe ConnMan daemon
 * presence or call libc. No parent wires.
 */
uint32_t
gj_connman_ok_u_10676(void)
{
	(void)NULL;
	return b10676_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_connman_ok_u_10676(void)
    __attribute__((alias("gj_connman_ok_u_10676")));
