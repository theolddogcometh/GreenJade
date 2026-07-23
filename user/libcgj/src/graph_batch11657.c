/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11657: session soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_session_ok_u_11657(void);
 *     - Returns 0 (session soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live user-session probe.
 *   uint32_t __gj_session_ok_u_11657  (alias)
 *   __libcgj_batch11657_marker = "libcgj-batch11657"
 *
 * Exclusive CREATE-ONLY (11651-11660: polkit/seatd soft id stubs). Unique
 * gj_session_ok_u_11657 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11657_marker[] = "libcgj-batch11657";

/* Soft lamp: session ok stub (not asserted). */
#define B11657_SESSION_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11657_session_ok(void)
{
	return B11657_SESSION_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_session_ok_u_11657 - session soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not enumerate sessions,
 * call sd-login, or probe display managers. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_session_ok_u_11657(void)
{
	(void)NULL;
	return b11657_session_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_session_ok_u_11657(void)
    __attribute__((alias("gj_session_ok_u_11657")));
