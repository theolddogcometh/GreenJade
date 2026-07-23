/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7525: waitid WNOWAIT option id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_waitid_wnowait_id_7525(void);
 *     - Return soft waitid WNOWAIT option bit (0x01000000).
 *   uint32_t __gj_waitid_wnowait_id_7525  (alias)
 *   __libcgj_batch7525_marker = "libcgj-batch7525"
 *
 * Exclusive continuum CREATE-ONLY (7521-7530: waitid options stubs —
 * wnohang_id, wexited_id, wstopped_id, wcontinued_id, wnowait_id,
 * has_wexited, has_wnohang, options_ok, options_errorish,
 * batch_id_7530).
 * Unique gj_waitid_wnowait_id_7525 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7525_marker[] = "libcgj-batch7525";

/*
 * Soft waitid WNOWAIT: leave the child in a waitable state (do not
 * reap). Matches Linux/glibc WNOWAIT without sys/wait.h.
 */
#define B7525_WAITID_WNOWAIT  0x01000000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7525_waitid_wnowait_id(void)
{
	return B7525_WAITID_WNOWAIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_waitid_wnowait_id_7525 - soft waitid WNOWAIT option constant.
 *
 * Always returns 0x01000000. Catalog id only; does not call waitid.
 * No parent wires.
 */
uint32_t
gj_waitid_wnowait_id_7525(void)
{
	(void)NULL;
	return b7525_waitid_wnowait_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_waitid_wnowait_id_7525(void)
    __attribute__((alias("gj_waitid_wnowait_id_7525")));
