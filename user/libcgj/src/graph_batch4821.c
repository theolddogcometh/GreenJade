/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4821: pack owner+seq into a lock token.
 *
 * Surface (unique symbols):
 *   uint64_t gj_lock_token_u(uint32_t owner, uint32_t seq);
 *     - Pack ((uint64_t)owner << 32) | seq into a lock token.
 *       High 32 = owner id; low 32 = sequence. Token 0 is free.
 *   uint64_t __gj_lock_token_u  (alias)
 *   __libcgj_batch4821_marker = "libcgj-batch4821"
 *
 * Exclusive continuum CREATE-ONLY (4821-4830: lock_token_u,
 * lock_owner_u, lock_seq_u, lock_is_free_u, lock_match_u,
 * lock_next_seq_u, lock_backoff_u, lock_deadline_u, lock_expired_u,
 * batch_id_4830). Unique gj_lock_token_u surface only; no multi-def.
 * Distinct from gj_slot_handle_u (batch4767). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4821_marker[] = "libcgj-batch4821";

/* ---- freestanding helpers ---------------------------------------------- */

/* Pack owner (high) and sequence (low) into a 64-bit lock token. */
static uint64_t
b4821_token(uint32_t u32Owner, uint32_t u32Seq)
{
	return ((uint64_t)u32Owner << 32) | (uint64_t)u32Seq;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lock_token_u - pack lock owner and sequence into a token.
 *
 * owner: high 32-bit owner id (0 means free when whole token is 0)
 * seq:   low 32-bit acquire/release sequence
 *
 * Returns ((uint64_t)owner << 32) | seq. Self-contained; no parent wires.
 */
uint64_t
gj_lock_token_u(uint32_t u32Owner, uint32_t u32Seq)
{
	(void)NULL;
	return b4821_token(u32Owner, u32Seq);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_lock_token_u(uint32_t u32Owner, uint32_t u32Seq)
    __attribute__((alias("gj_lock_token_u")));
