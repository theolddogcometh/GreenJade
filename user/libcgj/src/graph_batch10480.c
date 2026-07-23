/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10480: netmgr soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10480(void);
 *     - Returns the compile-time graph batch number for this TU (10480).
 *   uint32_t __gj_batch_id_10480  (alias)
 *   __libcgj_batch10480_marker = "libcgj-batch10480"
 *
 * Exclusive continuum CREATE-ONLY (10471-10480: netmgr soft id stubs —
 * bluez_ok_u_10471, networkmanager_ok_u_10472, iwd_ok_u_10473,
 * wpa_supplicant_ok_u_10474, modemmanager_ok_u_10475,
 * connman_ok_u_10476, avahi_ok_u_10477, resolvconf_ok_u_10478,
 * netmgr_soft_ready_u_10479, batch_id_10480). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10480_marker[] = "libcgj-batch10480";

#define B10480_BATCH_ID  10480u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10480_id(void)
{
	return B10480_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10480 - report this TU's graph batch number.
 *
 * Always returns 10480.
 */
uint32_t
gj_batch_id_10480(void)
{
	(void)NULL;
	return b10480_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10480(void)
    __attribute__((alias("gj_batch_id_10480")));
