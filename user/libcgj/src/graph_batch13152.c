/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13152: seatd ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_seatd_ok_u_13152(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       seatd probe for the auth soft continuum.
 *   uint32_t __gj_seatd_ok_u_13152  (alias)
 *   __libcgj_batch13152_marker = "libcgj-batch13152"
 *
 * Exclusive continuum CREATE-ONLY (13151-13160: auth soft id stubs —
 * polkit_ok_u_13151, seatd_ok_u_13152, elogind_ok_u_13153,
 * pam_ok_u_13154, shadow_ok_u_13155, sudo_ok_u_13156,
 * session_ok_u_13157, seat_ok_u_13158, auth_soft_ready_u_13159,
 * batch_id_13160). Unique surface only; no multi-def. Distinct from
 * gj_seatd_ok_u_12952 (batch12952), gj_seatd_ok_u_12752
 * (batch12752), and sibling auth soft ok_u stubs in this wave. No
 * parent wires. No __int128. No auth implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13152_marker[] = "libcgj-batch13152";

/* Soft seatd-ok lamp: always off (not a real seatd probe). */
#define B13152_SEATD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13152_seatd_ok(void)
{
	return B13152_SEATD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seatd_ok_u_13152 - seatd ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open seatd sockets or
 * call libc. No parent wires.
 */
uint32_t
gj_seatd_ok_u_13152(void)
{
	(void)NULL;
	return b13152_seatd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seatd_ok_u_13152(void)
    __attribute__((alias("gj_seatd_ok_u_13152")));
