/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7521: waitid WNOHANG option id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_waitid_wnohang_id_7521(void);
 *     - Return soft waitid WNOHANG option bit (1).
 *   uint32_t __gj_waitid_wnohang_id_7521  (alias)
 *   __libcgj_batch7521_marker = "libcgj-batch7521"
 *
 * Exclusive continuum CREATE-ONLY (7521-7530: waitid options stubs —
 * wnohang_id, wexited_id, wstopped_id, wcontinued_id, wnowait_id,
 * has_wexited, has_wnohang, options_ok, options_errorish,
 * batch_id_7530).
 * Unique gj_waitid_wnohang_id_7521 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7521_marker[] = "libcgj-batch7521";

/* Soft waitid WNOHANG: return immediately if no child has exited. */
#define B7521_WAITID_WNOHANG  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7521_waitid_wnohang_id(void)
{
	return B7521_WAITID_WNOHANG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_waitid_wnohang_id_7521 - soft waitid WNOHANG option constant.
 *
 * Always returns 1. Catalog id only; does not call waitid.
 * No parent wires.
 */
uint32_t
gj_waitid_wnohang_id_7521(void)
{
	(void)NULL;
	return b7521_waitid_wnohang_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_waitid_wnohang_id_7521(void)
    __attribute__((alias("gj_waitid_wnohang_id_7521")));
