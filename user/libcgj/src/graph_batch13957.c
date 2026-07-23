/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13957: session soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_session_ok_u_13957(void);
 *     - Returns 0 (session soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live session/login probe.
 *   uint32_t __gj_session_ok_u_13957  (alias)
 *   __libcgj_batch13957_marker = "libcgj-batch13957"
 *
 * Exclusive CREATE-ONLY (13951-13960: auth soft id stubs —
 * polkit_ok_u_13951, seatd_ok_u_13952, elogind_ok_u_13953,
 * pam_ok_u_13954, shadow_ok_u_13955, sudo_ok_u_13956,
 * session_ok_u_13957, seat_ok_u_13958, auth_soft_ready_u_13959,
 * batch_id_13960). Unique gj_session_ok_u_13957 surface only; no
 * multi-def. Distinct from prior auth soft waves
 * (gj_*_u_13751..13760, gj_*_u_13551..13560, gj_*_u_13351..13360,
 * gj_*_u_13151..13160) and sibling auth soft ok_u stubs in this wave.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13957_marker[] = "libcgj-batch13957";

/* Soft lamp: not asserted. */
#define B13957_SESSION_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13957_session_ok(void)
{
	return B13957_SESSION_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_session_ok_u_13957 - session soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not enumerate sessions,
 * call sd-login, or probe display managers. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_session_ok_u_13957(void)
{
	(void)NULL;
	return b13957_session_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_session_ok_u_13957(void)
    __attribute__((alias("gj_session_ok_u_13957")));
