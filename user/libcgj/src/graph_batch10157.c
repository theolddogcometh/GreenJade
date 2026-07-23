/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10157: session soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_session_ok_u_10157(void);
 *     - Returns 0 (session soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live user-session probe.
 *   uint32_t __gj_session_ok_u_10157  (alias)
 *   __libcgj_batch10157_marker = "libcgj-batch10157"
 *
 * Exclusive CREATE-ONLY (10151-10160: polkit/seatd soft id stubs). Unique
 * gj_session_ok_u_10157 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10157_marker[] = "libcgj-batch10157";

/* Soft lamp: session ok stub (not asserted). */
#define B10157_SESSION_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10157_session_ok(void)
{
	return B10157_SESSION_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_session_ok_u_10157 - session soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not enumerate sessions,
 * call sd-login, or probe display managers. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_session_ok_u_10157(void)
{
	(void)NULL;
	return b10157_session_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_session_ok_u_10157(void)
    __attribute__((alias("gj_session_ok_u_10157")));
