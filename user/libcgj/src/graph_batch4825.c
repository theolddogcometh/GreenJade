/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4825: match lock token owner field.
 *
 * Surface (unique symbols):
 *   int gj_lock_match_u(uint64_t tok, uint32_t owner);
 *     - Return 1 if the high-32 owner field of tok equals owner,
 *       else 0. Used to confirm unlock / reentrancy by holder.
 *   int __gj_lock_match_u  (alias)
 *   __libcgj_batch4825_marker = "libcgj-batch4825"
 *
 * Exclusive continuum CREATE-ONLY (4821-4830: lock_token_u,
 * lock_owner_u, lock_seq_u, lock_is_free_u, lock_match_u,
 * lock_next_seq_u, lock_backoff_u, lock_deadline_u, lock_expired_u,
 * batch_id_4830). Unique gj_lock_match_u surface only; no multi-def.
 * Distinct from gj_slot_match_u (batch4768). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4825_marker[] = "libcgj-batch4825";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff token high-32 owner equals expected owner. */
static int
b4825_match(uint64_t u64Tok, uint32_t u32Owner)
{
	uint32_t u32TokOwner;

	u32TokOwner = (uint32_t)(u64Tok >> 32);
	return (u32TokOwner == u32Owner) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lock_match_u - 1 if tok's packed owner equals owner.
 *
 * tok:   packed uint64_t (high=owner, low=seq)
 * owner: expected owner id
 *
 * Returns 1 on exact high-32 match, else 0. Self-contained; no parent
 * wires.
 */
int
gj_lock_match_u(uint64_t u64Tok, uint32_t u32Owner)
{
	(void)NULL;
	return b4825_match(u64Tok, u32Owner);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_lock_match_u(uint64_t u64Tok, uint32_t u32Owner)
    __attribute__((alias("gj_lock_match_u")));
