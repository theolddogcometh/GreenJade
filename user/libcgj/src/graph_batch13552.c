/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13552: seatd soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seatd_ok_u_13552(void);
 *     - Returns 0 (seatd soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live seatd/libseat probe.
 *   uint32_t __gj_seatd_ok_u_13552  (alias)
 *   __libcgj_batch13552_marker = "libcgj-batch13552"
 *
 * Exclusive CREATE-ONLY (13551-13560: auth soft id stubs —
 * polkit_ok_u_13551, seatd_ok_u_13552, elogind_ok_u_13553,
 * pam_ok_u_13554, shadow_ok_u_13555, sudo_ok_u_13556,
 * session_ok_u_13557, seat_ok_u_13558, auth_soft_ready_u_13559,
 * batch_id_13560). Unique gj_seatd_ok_u_13552 surface only; no
 * multi-def. Distinct from prior auth soft waves
 * (gj_*_u_13351..13360, gj_*_u_13151..13160, gj_*_u_12951..12960,
 * gj_*_u_12751..12760) and sibling auth soft ok_u stubs in this wave.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13552_marker[] = "libcgj-batch13552";

/* Soft lamp: not asserted. */
#define B13552_SEATD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13552_seatd_ok(void)
{
	return B13552_SEATD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seatd_ok_u_13552 - seatd soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not open seatd sockets,
 * call libseat, or probe seat managers. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_seatd_ok_u_13552(void)
{
	(void)NULL;
	return b13552_seatd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seatd_ok_u_13552(void)
    __attribute__((alias("gj_seatd_ok_u_13552")));
