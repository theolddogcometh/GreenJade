/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14270: netmgr soft stubs batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14270(void);
 *     - Returns the compile-time graph batch number for this TU (14270).
 *   uint32_t __gj_batch_id_14270  (alias)
 *   __libcgj_batch14270_marker = "libcgj-batch14270"
 *
 * Exclusive continuum CREATE-ONLY (14261-14270: netmgr soft stubs —
 * bluez_ok_u_14261, networkmanager_ok_u_14262, iwd_ok_u_14263,
 * wpa_supplicant_ok_u_14264, modemmanager_ok_u_14265,
 * connman_ok_u_14266, avahi_ok_u_14267, resolvconf_ok_u_14268,
 * netmgr_soft_ready_u_14269, batch_id_14270). Unique gj_batch_id_14270
 * surface only; no multi-def. Does NOT redefine gj_batch_id / prior
 * batch_id_* symbols - avoid multi-def. Closes the 14261-14270
 * continuum. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14270_marker[] = "libcgj-batch14270";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14270_id(void)
{
	return 14270u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14270 - report this TU's graph batch number.
 *
 * Always returns 14270. Soft pure-data identity for the netmgr soft
 * stubs continuum close. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_14270(void)
{
	(void)NULL;
	return b14270_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14270(void)
    __attribute__((alias("gj_batch_id_14270")));
