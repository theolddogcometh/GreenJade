/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4823: extract sequence from a lock token.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lock_seq_u(uint64_t tok);
 *     - Return the low 32 bits of tok (sequence field).
 *   uint32_t __gj_lock_seq_u  (alias)
 *   __libcgj_batch4823_marker = "libcgj-batch4823"
 *
 * Exclusive continuum CREATE-ONLY (4821-4830: lock_token_u,
 * lock_owner_u, lock_seq_u, lock_is_free_u, lock_match_u,
 * lock_next_seq_u, lock_backoff_u, lock_deadline_u, lock_expired_u,
 * batch_id_4830). Unique gj_lock_seq_u surface only; no multi-def.
 * Distinct from gj_slot_index_u (batch4765). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4823_marker[] = "libcgj-batch4823";

/* ---- freestanding helpers ---------------------------------------------- */

/* Low 32 bits of a packed (owner:seq) lock token. */
static uint32_t
b4823_seq(uint64_t u64Tok)
{
	return (uint32_t)u64Tok;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lock_seq_u - extract sequence (low 32) from a packed lock token.
 *
 * tok: packed uint64_t (high=owner, low=seq)
 *
 * Returns (uint32_t)tok. Self-contained; no parent wires.
 */
uint32_t
gj_lock_seq_u(uint64_t u64Tok)
{
	(void)NULL;
	return b4823_seq(u64Tok);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lock_seq_u(uint64_t u64Tok)
    __attribute__((alias("gj_lock_seq_u")));
