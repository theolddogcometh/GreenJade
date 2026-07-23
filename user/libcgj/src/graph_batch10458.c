/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10458: seat ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_seat_ok_u_10458(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       seat probe for the auth soft continuum.
 *   uint32_t __gj_seat_ok_u_10458  (alias)
 *   __libcgj_batch10458_marker = "libcgj-batch10458"
 *
 * Exclusive continuum CREATE-ONLY (10451-10460: auth soft id stubs —
 * polkit_ok_u_10451, seatd_ok_u_10452, elogind_ok_u_10453,
 * pam_ok_u_10454, shadow_ok_u_10455, sudo_ok_u_10456,
 * session_ok_u_10457, seat_ok_u_10458, auth_soft_ready_u_10459,
 * batch_id_10460). Unique surface only; no multi-def. No parent wires.
 * No __int128. No auth implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10458_marker[] = "libcgj-batch10458";

/* Soft seat-ok lamp: always off (not a real seat probe). */
#define B10458_SEAT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10458_seat_ok(void)
{
	return B10458_SEAT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seat_ok_u_10458 - seat ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not query seats or call
 * libc. No parent wires.
 */
uint32_t
gj_seat_ok_u_10458(void)
{
	(void)NULL;
	return b10458_seat_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seat_ok_u_10458(void)
    __attribute__((alias("gj_seat_ok_u_10458")));
