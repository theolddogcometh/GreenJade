/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7557: sigprocmask how validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sigprocmask_how_ok_u_7557(uint32_t how);
 *     - Return 1 if how is a known soft how code
 *       (SIG_BLOCK|SIG_UNBLOCK|SIG_SETMASK), else 0.
 *   uint32_t __gj_sigprocmask_how_ok_u_7557  (alias)
 *   __libcgj_batch7557_marker = "libcgj-batch7557"
 *
 * Exclusive continuum CREATE-ONLY (7551-7560: sigprocmask how stubs —
 * block_id, unblock_id, setmask_id, is_block, is_unblock, is_setmask,
 * how_ok, how_pack, how_errorish, continuum + batch_id_7560). Unique
 * gj_sigprocmask_how_ok_u_7557 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7557_marker[] = "libcgj-batch7557";

/* Soft known how codes: SIG_BLOCK=0, SIG_UNBLOCK=1, SIG_SETMASK=2. */
#define B7557_SIG_BLOCK    ((uint32_t)0u)
#define B7557_SIG_UNBLOCK  ((uint32_t)1u)
#define B7557_SIG_SETMASK  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7557_how_ok(uint32_t u32How)
{
	if (u32How == B7557_SIG_BLOCK) {
		return 1u;
	}
	if (u32How == B7557_SIG_UNBLOCK) {
		return 1u;
	}
	if (u32How == B7557_SIG_SETMASK) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sigprocmask_how_ok_u_7557 - 1 if how is a known sigprocmask how.
 *
 * how: soft sigprocmask(2) how argument
 *
 * Returns 1 when how is 0, 1, or 2; else 0. Soft catalog check; does
 * not call sigprocmask. No parent wires.
 */
uint32_t
gj_sigprocmask_how_ok_u_7557(uint32_t u32How)
{
	(void)NULL;
	return b7557_how_ok(u32How);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sigprocmask_how_ok_u_7557(uint32_t u32How)
    __attribute__((alias("gj_sigprocmask_how_ok_u_7557")));
