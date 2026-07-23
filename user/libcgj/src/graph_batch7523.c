/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7523: waitid WSTOPPED option id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_waitid_wstopped_id_7523(void);
 *     - Return soft waitid WSTOPPED option bit (2).
 *   uint32_t __gj_waitid_wstopped_id_7523  (alias)
 *   __libcgj_batch7523_marker = "libcgj-batch7523"
 *
 * Exclusive continuum CREATE-ONLY (7521-7530: waitid options stubs —
 * wnohang_id, wexited_id, wstopped_id, wcontinued_id, wnowait_id,
 * has_wexited, has_wnohang, options_ok, options_errorish,
 * batch_id_7530).
 * Unique gj_waitid_wstopped_id_7523 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7523_marker[] = "libcgj-batch7523";

/* Soft waitid WSTOPPED: wait for children that have stopped. */
#define B7523_WAITID_WSTOPPED  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7523_waitid_wstopped_id(void)
{
	return B7523_WAITID_WSTOPPED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_waitid_wstopped_id_7523 - soft waitid WSTOPPED option constant.
 *
 * Always returns 2. Catalog id only; does not call waitid.
 * No parent wires.
 */
uint32_t
gj_waitid_wstopped_id_7523(void)
{
	(void)NULL;
	return b7523_waitid_wstopped_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_waitid_wstopped_id_7523(void)
    __attribute__((alias("gj_waitid_wstopped_id_7523")));
