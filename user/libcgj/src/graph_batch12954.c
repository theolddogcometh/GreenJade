/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12954: pam ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pam_ok_u_12954(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PAM probe for the auth soft continuum.
 *   uint32_t __gj_pam_ok_u_12954  (alias)
 *   __libcgj_batch12954_marker = "libcgj-batch12954"
 *
 * Exclusive continuum CREATE-ONLY (12951-12960: auth soft id stubs —
 * polkit_ok_u_12951, seatd_ok_u_12952, elogind_ok_u_12953,
 * pam_ok_u_12954, shadow_ok_u_12955, sudo_ok_u_12956,
 * session_ok_u_12957, seat_ok_u_12958, auth_soft_ready_u_12959,
 * batch_id_12960). Unique surface only; no multi-def. No parent wires.
 * No __int128. No auth implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12954_marker[] = "libcgj-batch12954";

/* Soft pam-ok lamp: always off (not a real PAM probe). */
#define B12954_PAM_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12954_pam_ok(void)
{
	return B12954_PAM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pam_ok_u_12954 - pam ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not invoke PAM or call
 * libc. No parent wires.
 */
uint32_t
gj_pam_ok_u_12954(void)
{
	(void)NULL;
	return b12954_pam_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pam_ok_u_12954(void)
    __attribute__((alias("gj_pam_ok_u_12954")));
