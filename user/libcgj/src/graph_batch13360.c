/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13360: auth soft id stubs batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13360(void);
 *     - Returns the compile-time graph batch number for this TU (13360).
 *   uint32_t __gj_batch_id_13360  (alias)
 *   __libcgj_batch13360_marker = "libcgj-batch13360"
 *
 * Exclusive CREATE-ONLY (13351-13360: auth soft id stubs —
 * polkit_ok_u_13351, seatd_ok_u_13352, elogind_ok_u_13353,
 * pam_ok_u_13354, shadow_ok_u_13355, sudo_ok_u_13356,
 * session_ok_u_13357, seat_ok_u_13358, auth_soft_ready_u_13359,
 * batch_id_13360). Unique gj_batch_id_13360 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_batch_id_13300 / prior batch_id_*
 * symbols - avoid multi-def. Closes the 13351-13360 continuum
 * (polkit/seatd/elogind/pam/shadow/sudo/session/seat soft ok stubs +
 * auth soft ready). Distinct from gj_batch_id_13160,
 * gj_batch_id_12960, and gj_batch_id_12760. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13360_marker[] = "libcgj-batch13360";

#define B13360_BATCH_ID  13360u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13360_id(void)
{
	return B13360_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13360 - report this TU's graph batch number.
 *
 * Always returns 13360. Soft pure-data identity for the auth soft id
 * stubs continuum close. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_13360(void)
{
	(void)NULL;
	return b13360_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13360(void)
    __attribute__((alias("gj_batch_id_13360")));
