/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13560: auth soft id stubs batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13560(void);
 *     - Returns the compile-time graph batch number for this TU (13560).
 *   uint32_t __gj_batch_id_13560  (alias)
 *   __libcgj_batch13560_marker = "libcgj-batch13560"
 *
 * Exclusive CREATE-ONLY (13551-13560: auth soft id stubs —
 * polkit_ok_u_13551, seatd_ok_u_13552, elogind_ok_u_13553,
 * pam_ok_u_13554, shadow_ok_u_13555, sudo_ok_u_13556,
 * session_ok_u_13557, seat_ok_u_13558, auth_soft_ready_u_13559,
 * batch_id_13560). Unique gj_batch_id_13560 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_batch_id_13500 / prior batch_id_*
 * symbols - avoid multi-def. Closes the 13551-13560 continuum
 * (polkit/seatd/elogind/pam/shadow/sudo/session/seat soft ok stubs +
 * auth soft ready). Distinct from gj_batch_id_13360,
 * gj_batch_id_13160, gj_batch_id_12960, and gj_batch_id_12760. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13560_marker[] = "libcgj-batch13560";

#define B13560_BATCH_ID  13560u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13560_id(void)
{
	return B13560_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13560 - report this TU's graph batch number.
 *
 * Always returns 13560. Soft pure-data identity for the auth soft id
 * stubs continuum close. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_13560(void)
{
	(void)NULL;
	return b13560_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13560(void)
    __attribute__((alias("gj_batch_id_13560")));
