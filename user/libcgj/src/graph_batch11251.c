/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11251: polkit ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_polkit_ok_u_11251(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       polkit/auth probe for the auth soft continuum.
 *   uint32_t __gj_polkit_ok_u_11251  (alias)
 *   __libcgj_batch11251_marker = "libcgj-batch11251"
 *
 * Exclusive continuum CREATE-ONLY (11251-11260: auth soft id stubs —
 * polkit_ok_u_11251, seatd_ok_u_11252, elogind_ok_u_11253,
 * pam_ok_u_11254, shadow_ok_u_11255, sudo_ok_u_11256,
 * session_ok_u_11257, seat_ok_u_11258, auth_soft_ready_u_11259,
 * batch_id_11260). Unique surface only; no multi-def. Distinct from
 * gj_polkit_ok_u_11051 (batch11051), gj_polkit_ok_u_10851 (batch10851),
 * gj_polkit_ok_u_10651 (batch10651), and gj_polkit_ok_u_10451
 * (batch10451). No parent wires. No __int128. No auth implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11251_marker[] = "libcgj-batch11251";

/* Soft polkit-ok lamp: always off (not a real polkit/auth probe). */
#define B11251_POLKIT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11251_polkit_ok(void)
{
	return B11251_POLKIT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_polkit_ok_u_11251 - polkit ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe polkit/auth or
 * call libc. No parent wires.
 */
uint32_t
gj_polkit_ok_u_11251(void)
{
	(void)NULL;
	return b11251_polkit_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_polkit_ok_u_11251(void)
    __attribute__((alias("gj_polkit_ok_u_11251")));
