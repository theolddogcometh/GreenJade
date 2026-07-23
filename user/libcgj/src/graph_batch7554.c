/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7554: sigprocmask how is-SIG_BLOCK stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sigprocmask_is_block_u_7554(uint32_t how);
 *     - Return 1 if how == SIG_BLOCK (0), else 0.
 *   uint32_t __gj_sigprocmask_is_block_u_7554  (alias)
 *   __libcgj_batch7554_marker = "libcgj-batch7554"
 *
 * Exclusive continuum CREATE-ONLY (7551-7560: sigprocmask how stubs —
 * block_id, unblock_id, setmask_id, is_block, is_unblock, is_setmask,
 * how_ok, how_pack, how_errorish, continuum + batch_id_7560). Unique
 * gj_sigprocmask_is_block_u_7554 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7554_marker[] = "libcgj-batch7554";

/* Soft SIG_BLOCK how code. */
#define B7554_SIG_BLOCK  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7554_is_block(uint32_t u32How)
{
	return (u32How == B7554_SIG_BLOCK) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sigprocmask_is_block_u_7554 - 1 if how is SIG_BLOCK.
 *
 * how: soft sigprocmask(2) how argument
 *
 * Returns 1 when how equals 0 (SIG_BLOCK), else 0. Pure compare; does
 * not call sigprocmask. No parent wires.
 */
uint32_t
gj_sigprocmask_is_block_u_7554(uint32_t u32How)
{
	(void)NULL;
	return b7554_is_block(u32How);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sigprocmask_is_block_u_7554(uint32_t u32How)
    __attribute__((alias("gj_sigprocmask_is_block_u_7554")));
