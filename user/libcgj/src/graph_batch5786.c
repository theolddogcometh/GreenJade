/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5786: user session count on seat0.
 *
 * Surface (unique symbols):
 *   uint32_t gj_user_session_count_5786(void);
 *     - Returns stub user session count bound to seat0 (1).
 *   uint32_t __gj_user_session_count_5786  (alias)
 *   __libcgj_batch5786_marker = "libcgj-batch5786"
 *
 * Exclusive continuum CREATE-ONLY (5781-5790: user session seats,
 * logind stubs, seat0 active). Unique gj_user_session_count_5786
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5786_marker[] = "libcgj-batch5786";

/* One active user session on seat0 in the stub topology. */
#define B5786_USER_SESSION_COUNT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5786_user_session_count(void)
{
	return B5786_USER_SESSION_COUNT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_user_session_count_5786 - report stub user session count on seat0.
 *
 * Always returns 1. Integer-only logind session list length; does not
 * enumerate /run/systemd/sessions. Does not call libc. No parent wires.
 */
uint32_t
gj_user_session_count_5786(void)
{
	(void)NULL;
	return b5786_user_session_count();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_user_session_count_5786(void)
    __attribute__((alias("gj_user_session_count_5786")));
