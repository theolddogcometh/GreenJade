/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7555: sigprocmask how is-SIG_UNBLOCK stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sigprocmask_is_unblock_u_7555(uint32_t how);
 *     - Return 1 if how == SIG_UNBLOCK (1), else 0.
 *   uint32_t __gj_sigprocmask_is_unblock_u_7555  (alias)
 *   __libcgj_batch7555_marker = "libcgj-batch7555"
 *
 * Exclusive continuum CREATE-ONLY (7551-7560: sigprocmask how stubs —
 * block_id, unblock_id, setmask_id, is_block, is_unblock, is_setmask,
 * how_ok, how_pack, how_errorish, continuum + batch_id_7560). Unique
 * gj_sigprocmask_is_unblock_u_7555 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7555_marker[] = "libcgj-batch7555";

/* Soft SIG_UNBLOCK how code. */
#define B7555_SIG_UNBLOCK  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7555_is_unblock(uint32_t u32How)
{
	return (u32How == B7555_SIG_UNBLOCK) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sigprocmask_is_unblock_u_7555 - 1 if how is SIG_UNBLOCK.
 *
 * how: soft sigprocmask(2) how argument
 *
 * Returns 1 when how equals 1 (SIG_UNBLOCK), else 0. Pure compare; does
 * not call sigprocmask. No parent wires.
 */
uint32_t
gj_sigprocmask_is_unblock_u_7555(uint32_t u32How)
{
	(void)NULL;
	return b7555_is_unblock(u32How);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sigprocmask_is_unblock_u_7555(uint32_t u32How)
    __attribute__((alias("gj_sigprocmask_is_unblock_u_7555")));
