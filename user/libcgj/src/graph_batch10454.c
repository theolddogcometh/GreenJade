/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10454: pam ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pam_ok_u_10454(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PAM probe for the auth soft continuum.
 *   uint32_t __gj_pam_ok_u_10454  (alias)
 *   __libcgj_batch10454_marker = "libcgj-batch10454"
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

const char __libcgj_batch10454_marker[] = "libcgj-batch10454";

/* Soft pam-ok lamp: always off (not a real PAM probe). */
#define B10454_PAM_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10454_pam_ok(void)
{
	return B10454_PAM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pam_ok_u_10454 - pam ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not invoke PAM or call
 * libc. No parent wires.
 */
uint32_t
gj_pam_ok_u_10454(void)
{
	(void)NULL;
	return b10454_pam_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pam_ok_u_10454(void)
    __attribute__((alias("gj_pam_ok_u_10454")));
