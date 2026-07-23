/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11256: sudo ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sudo_ok_u_11256(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       sudo probe for the auth soft continuum.
 *   uint32_t __gj_sudo_ok_u_11256  (alias)
 *   __libcgj_batch11256_marker = "libcgj-batch11256"
 *
 * Exclusive continuum CREATE-ONLY (11251-11260: auth soft id stubs —
 * polkit_ok_u_11251, seatd_ok_u_11252, elogind_ok_u_11253,
 * pam_ok_u_11254, shadow_ok_u_11255, sudo_ok_u_11256,
 * session_ok_u_11257, seat_ok_u_11258, auth_soft_ready_u_11259,
 * batch_id_11260). Unique surface only; no multi-def. Distinct from
 * gj_sudo_ok_u_11056 (batch11056), gj_sudo_ok_u_10856 (batch10856), and
 * gj_sudo_ok_u_10656 (batch10656). No parent wires. No __int128. No
 * auth implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11256_marker[] = "libcgj-batch11256";

/* Soft sudo-ok lamp: always off (not a real sudo probe). */
#define B11256_SUDO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11256_sudo_ok(void)
{
	return B11256_SUDO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sudo_ok_u_11256 - sudo ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe sudo or
 * call libc. No parent wires.
 */
uint32_t
gj_sudo_ok_u_11256(void)
{
	(void)NULL;
	return b11256_sudo_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sudo_ok_u_11256(void)
    __attribute__((alias("gj_sudo_ok_u_11256")));
