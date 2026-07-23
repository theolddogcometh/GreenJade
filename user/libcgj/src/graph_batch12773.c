/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12773: iwd ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_iwd_ok_u_12773(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       iwd runtime probe for the netmgr soft path.
 *   uint32_t __gj_iwd_ok_u_12773  (alias)
 *   __libcgj_batch12773_marker = "libcgj-batch12773"
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

const char __libcgj_batch12773_marker[] = "libcgj-batch12773";

/* Soft iwd-ok lamp: always off (not a real runtime probe). */
#define B12773_IWD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12773_ok(void)
{
	return B12773_IWD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iwd_ok_u_12773 - iwd ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe iwd presence
 * or call libc. No parent wires.
 */
uint32_t
gj_iwd_ok_u_12773(void)
{
	(void)NULL;
	return b12773_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iwd_ok_u_12773(void)
    __attribute__((alias("gj_iwd_ok_u_12773")));
