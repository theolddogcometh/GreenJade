/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13154: pam ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pam_ok_u_13154(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PAM probe for the auth soft continuum.
 *   uint32_t __gj_pam_ok_u_13154  (alias)
 *   __libcgj_batch13154_marker = "libcgj-batch13154"
 *
 * Exclusive continuum CREATE-ONLY (13151-13160: auth soft id stubs —
 * polkit_ok_u_13151, seatd_ok_u_13152, elogind_ok_u_13153,
 * pam_ok_u_13154, shadow_ok_u_13155, sudo_ok_u_13156,
 * session_ok_u_13157, seat_ok_u_13158, auth_soft_ready_u_13159,
 * batch_id_13160). Unique surface only; no multi-def. Distinct from
 * gj_pam_ok_u_12954 (batch12954), gj_pam_ok_u_12754 (batch12754), and
 * sibling auth soft ok_u stubs in this wave. No parent wires. No
 * __int128. No auth implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13154_marker[] = "libcgj-batch13154";

/* Soft pam-ok lamp: always off (not a real PAM probe). */
#define B13154_PAM_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13154_pam_ok(void)
{
	return B13154_PAM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pam_ok_u_13154 - pam ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not invoke PAM or call
 * libc. No parent wires.
 */
uint32_t
gj_pam_ok_u_13154(void)
{
	(void)NULL;
	return b13154_pam_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pam_ok_u_13154(void)
    __attribute__((alias("gj_pam_ok_u_13154")));
