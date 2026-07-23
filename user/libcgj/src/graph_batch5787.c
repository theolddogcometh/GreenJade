/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5787: active session id on seat0.
 *
 * Surface (unique symbols):
 *   uint32_t gj_active_session_id_5787(void);
 *     - Returns stub active session id (1).
 *   uint32_t __gj_active_session_id_5787  (alias)
 *   __libcgj_batch5787_marker = "libcgj-batch5787"
 *
 * Exclusive continuum CREATE-ONLY (5781-5790: user session seats,
 * logind stubs, seat0 active). Unique gj_active_session_id_5787
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5787_marker[] = "libcgj-batch5787";

/* Active session id bound to seat0 in this stub wave. */
#define B5787_ACTIVE_SESSION_ID  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5787_active_session_id(void)
{
	return B5787_ACTIVE_SESSION_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_active_session_id_5787 - report stub active session id on seat0.
 *
 * Always returns 1. Integer-only logind session id stub; does not
 * parse XDG_SESSION_ID or call sd_seat_get_active. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_active_session_id_5787(void)
{
	(void)NULL;
	return b5787_active_session_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_active_session_id_5787(void)
    __attribute__((alias("gj_active_session_id_5787")));
