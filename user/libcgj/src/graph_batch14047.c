/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14047: session soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_session_ok_u_14047(void);
 *     - Returns 0 (session soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live user-session probe.
 *   uint32_t __gj_session_ok_u_14047  (alias)
 *   __libcgj_batch14047_marker = "libcgj-batch14047"
 *
 * Exclusive continuum CREATE-ONLY (14041-14050: polkit/seatd soft id
 * stubs — polkit_ok_u_14041, seatd_ok_u_14042, elogind_ok_u_14043,
 * pam_ok_u_14044, shadow_ok_u_14045, sudo_ok_u_14046,
 * session_ok_u_14047, seat_ok_u_14048, auth_soft_ready_u_14049,
 * batch_id_14050). Unique gj_session_ok_u_14047 surface only; no
 * multi-def. Distinct from gj_session_ok_u_10157. No parent wires.
 * No __int128. No session-manager implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14047_marker[] = "libcgj-batch14047";

/* Soft lamp: session ok stub (not asserted). */
#define B14047_SESSION_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14047_session_ok(void)
{
	return B14047_SESSION_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_session_ok_u_14047 - session soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not enumerate sessions,
 * call sd-login, or probe display managers. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_session_ok_u_14047(void)
{
	(void)NULL;
	return b14047_session_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_session_ok_u_14047(void)
    __attribute__((alias("gj_session_ok_u_14047")));
