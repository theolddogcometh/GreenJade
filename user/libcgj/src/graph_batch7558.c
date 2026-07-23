/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7558: sigprocmask how pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sigprocmask_how_pack_u_7558(uint32_t want_block,
 *                                           uint32_t want_unblock,
 *                                           uint32_t want_setmask);
 *     - Soft pack: non-zero want_block → bit0 (1), want_unblock → bit1
 *       (2), want_setmask → bit2 (4); OR selected presence bits.
 *   uint32_t __gj_sigprocmask_how_pack_u_7558  (alias)
 *   __libcgj_batch7558_marker = "libcgj-batch7558"
 *
 * Exclusive continuum CREATE-ONLY (7551-7560: sigprocmask how stubs —
 * block_id, unblock_id, setmask_id, is_block, is_unblock, is_setmask,
 * how_ok, how_pack, how_errorish, continuum + batch_id_7560). Unique
 * gj_sigprocmask_how_pack_u_7558 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7558_marker[] = "libcgj-batch7558";

/* Soft presence bits for packed how-kind catalog (not the how codes). */
#define B7558_PACK_BLOCK    ((uint32_t)0x00000001u)
#define B7558_PACK_UNBLOCK  ((uint32_t)0x00000002u)
#define B7558_PACK_SETMASK  ((uint32_t)0x00000004u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7558_how_pack(uint32_t u32WantBlock, uint32_t u32WantUnblock,
    uint32_t u32WantSetmask)
{
	uint32_t u32Out = 0u;

	if (u32WantBlock != 0u) {
		u32Out |= B7558_PACK_BLOCK;
	}
	if (u32WantUnblock != 0u) {
		u32Out |= B7558_PACK_UNBLOCK;
	}
	if (u32WantSetmask != 0u) {
		u32Out |= B7558_PACK_SETMASK;
	}
	return u32Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sigprocmask_how_pack_u_7558 - soft pack of how-kind request bits.
 *
 * want_block:   non-zero to include SIG_BLOCK presence bit
 * want_unblock: non-zero to include SIG_UNBLOCK presence bit
 * want_setmask: non-zero to include SIG_SETMASK presence bit
 *
 * Returns packed presence mask (bits 0..2). Pure integer; does not call
 * sigprocmask. No parent wires.
 */
uint32_t
gj_sigprocmask_how_pack_u_7558(uint32_t want_block, uint32_t want_unblock,
    uint32_t want_setmask)
{
	(void)NULL;
	return b7558_how_pack(want_block, want_unblock, want_setmask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sigprocmask_how_pack_u_7558(uint32_t want_block,
    uint32_t want_unblock, uint32_t want_setmask)
    __attribute__((alias("gj_sigprocmask_how_pack_u_7558")));
