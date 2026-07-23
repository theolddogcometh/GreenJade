/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10657: session ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_session_ok_u_10657(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       session probe for the auth soft continuum.
 *   uint32_t __gj_session_ok_u_10657  (alias)
 *   __libcgj_batch10657_marker = "libcgj-batch10657"
 *
 * Exclusive continuum CREATE-ONLY (10651-10660: auth soft id stubs —
 * polkit_ok_u_10651, seatd_ok_u_10652, elogind_ok_u_10653,
 * pam_ok_u_10654, shadow_ok_u_10655, sudo_ok_u_10656,
 * session_ok_u_10657, seat_ok_u_10658, auth_soft_ready_u_10659,
 * batch_id_10660). Unique surface only; no multi-def. Distinct from
 * gj_session_ok_u_10457 (batch10457). No parent wires. No __int128. No
 * auth implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10657_marker[] = "libcgj-batch10657";

/* Soft session-ok lamp: always off (not a real session probe). */
#define B10657_SESSION_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10657_session_ok(void)
{
	return B10657_SESSION_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_session_ok_u_10657 - session ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not query sessions or
 * call libc. No parent wires.
 */
uint32_t
gj_session_ok_u_10657(void)
{
	(void)NULL;
	return b10657_session_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_session_ok_u_10657(void)
    __attribute__((alias("gj_session_ok_u_10657")));
