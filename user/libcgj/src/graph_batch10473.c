/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10473: iwd ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_iwd_ok_u_10473(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       iwd (iNet wireless daemon) runtime probe for the netmgr soft path.
 *   uint32_t __gj_iwd_ok_u_10473  (alias)
 *   __libcgj_batch10473_marker = "libcgj-batch10473"
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

const char __libcgj_batch10473_marker[] = "libcgj-batch10473";

/* Soft iwd-ok lamp: always off (not a real runtime probe). */
#define B10473_IWD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10473_ok(void)
{
	return B10473_IWD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iwd_ok_u_10473 - iwd ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe iwd daemon
 * presence or call libc. No parent wires.
 */
uint32_t
gj_iwd_ok_u_10473(void)
{
	(void)NULL;
	return b10473_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iwd_ok_u_10473(void)
    __attribute__((alias("gj_iwd_ok_u_10473")));
