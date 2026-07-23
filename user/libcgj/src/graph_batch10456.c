/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10456: sudo ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sudo_ok_u_10456(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       sudo probe for the auth soft continuum.
 *   uint32_t __gj_sudo_ok_u_10456  (alias)
 *   __libcgj_batch10456_marker = "libcgj-batch10456"
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

const char __libcgj_batch10456_marker[] = "libcgj-batch10456";

/* Soft sudo-ok lamp: always off (not a real sudo probe). */
#define B10456_SUDO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10456_sudo_ok(void)
{
	return B10456_SUDO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sudo_ok_u_10456 - sudo ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not invoke sudo or call
 * libc. No parent wires.
 */
uint32_t
gj_sudo_ok_u_10456(void)
{
	(void)NULL;
	return b10456_sudo_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sudo_ok_u_10456(void)
    __attribute__((alias("gj_sudo_ok_u_10456")));
