/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10856: sudo ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sudo_ok_u_10856(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       sudo probe for the auth soft continuum.
 *   uint32_t __gj_sudo_ok_u_10856  (alias)
 *   __libcgj_batch10856_marker = "libcgj-batch10856"
 *
 * Exclusive continuum CREATE-ONLY (10851-10860: auth soft id stubs —
 * polkit_ok_u_10851, seatd_ok_u_10852, elogind_ok_u_10853,
 * pam_ok_u_10854, shadow_ok_u_10855, sudo_ok_u_10856,
 * session_ok_u_10857, seat_ok_u_10858, auth_soft_ready_u_10859,
 * batch_id_10860). Unique surface only; no multi-def. Distinct from
 * gj_sudo_ok_u_10656 (batch10656) and gj_sudo_ok_u_10456
 * (batch10456). No parent wires. No __int128. No auth implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10856_marker[] = "libcgj-batch10856";

/* Soft sudo-ok lamp: always off (not a real sudo probe). */
#define B10856_SUDO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10856_sudo_ok(void)
{
	return B10856_SUDO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sudo_ok_u_10856 - sudo ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not invoke sudo or call
 * libc. No parent wires.
 */
uint32_t
gj_sudo_ok_u_10856(void)
{
	(void)NULL;
	return b10856_sudo_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sudo_ok_u_10856(void)
    __attribute__((alias("gj_sudo_ok_u_10856")));
