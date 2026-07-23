/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13356: sudo soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sudo_ok_u_13356(void);
 *     - Returns 0 (sudo soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live sudo/sudoers probe.
 *   uint32_t __gj_sudo_ok_u_13356  (alias)
 *   __libcgj_batch13356_marker = "libcgj-batch13356"
 *
 * Exclusive CREATE-ONLY (13351-13360: auth soft id stubs —
 * polkit_ok_u_13351, seatd_ok_u_13352, elogind_ok_u_13353,
 * pam_ok_u_13354, shadow_ok_u_13355, sudo_ok_u_13356,
 * session_ok_u_13357, seat_ok_u_13358, auth_soft_ready_u_13359,
 * batch_id_13360). Unique gj_sudo_ok_u_13356 surface only; no
 * multi-def. Distinct from prior auth soft waves
 * (gj_*_u_13151..13160, gj_*_u_12951..12960, gj_*_u_12751..12760)
 * and sibling auth soft ok_u stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13356_marker[] = "libcgj-batch13356";

/* Soft lamp: not asserted. */
#define B13356_SUDO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13356_sudo_ok(void)
{
	return B13356_SUDO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sudo_ok_u_13356 - sudo soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not open sudoers,
 * invoke sudo, or probe privilege escalation paths. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_sudo_ok_u_13356(void)
{
	(void)NULL;
	return b13356_sudo_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sudo_ok_u_13356(void)
    __attribute__((alias("gj_sudo_ok_u_13356")));
