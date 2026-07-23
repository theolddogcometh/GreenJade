/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12556: sudo soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sudo_ok_u_12556(void);
 *     - Returns 0 (sudo soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live sudo/sudoers probe.
 *   uint32_t __gj_sudo_ok_u_12556  (alias)
 *   __libcgj_batch12556_marker = "libcgj-batch12556"
 *
 * Exclusive CREATE-ONLY (12551-12560: auth soft id stubs —
 * polkit_ok_u_12551, seatd_ok_u_12552, elogind_ok_u_12553,
 * pam_ok_u_12554, shadow_ok_u_12555, sudo_ok_u_12556,
 * session_ok_u_12557, seat_ok_u_12558, auth_soft_ready_u_12559,
 * batch_id_12560). Unique gj_sudo_ok_u_12556 surface only; no
 * multi-def. Distinct from prior auth soft waves
 * (gj_*_u_12351..12360, gj_*_u_12151..12160, gj_*_u_11851..11860)
 * and sibling auth soft ok_u stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12556_marker[] = "libcgj-batch12556";

/* Soft lamp: not asserted. */
#define B12556_SUDO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12556_sudo_ok(void)
{
	return B12556_SUDO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sudo_ok_u_12556 - sudo soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not invoke sudo, parse
 * sudoers, or probe privilege escalation paths. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_sudo_ok_u_12556(void)
{
	(void)NULL;
	return b12556_sudo_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sudo_ok_u_12556(void)
    __attribute__((alias("gj_sudo_ok_u_12556")));
