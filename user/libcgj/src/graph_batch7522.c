/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7522: waitid WEXITED option id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_waitid_wexited_id_7522(void);
 *     - Return soft waitid WEXITED option bit (4).
 *   uint32_t __gj_waitid_wexited_id_7522  (alias)
 *   __libcgj_batch7522_marker = "libcgj-batch7522"
 *
 * Exclusive continuum CREATE-ONLY (7521-7530: waitid options stubs —
 * wnohang_id, wexited_id, wstopped_id, wcontinued_id, wnowait_id,
 * has_wexited, has_wnohang, options_ok, options_errorish,
 * batch_id_7530).
 * Unique gj_waitid_wexited_id_7522 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7522_marker[] = "libcgj-batch7522";

/* Soft waitid WEXITED: wait for children that have exited. */
#define B7522_WAITID_WEXITED  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7522_waitid_wexited_id(void)
{
	return B7522_WAITID_WEXITED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_waitid_wexited_id_7522 - soft waitid WEXITED option constant.
 *
 * Always returns 4. Catalog id only; does not call waitid.
 * No parent wires.
 */
uint32_t
gj_waitid_wexited_id_7522(void)
{
	(void)NULL;
	return b7522_waitid_wexited_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_waitid_wexited_id_7522(void)
    __attribute__((alias("gj_waitid_wexited_id_7522")));
