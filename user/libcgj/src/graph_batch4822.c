/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4822: extract owner from a lock token.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lock_owner_u(uint64_t tok);
 *     - Return the high 32 bits of tok (owner field).
 *   uint32_t __gj_lock_owner_u  (alias)
 *   __libcgj_batch4822_marker = "libcgj-batch4822"
 *
 * Exclusive continuum CREATE-ONLY (4821-4830: lock_token_u,
 * lock_owner_u, lock_seq_u, lock_is_free_u, lock_match_u,
 * lock_next_seq_u, lock_backoff_u, lock_deadline_u, lock_expired_u,
 * batch_id_4830). Unique gj_lock_owner_u surface only; no multi-def.
 * Distinct from gj_slot_gen_u (batch4766). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4822_marker[] = "libcgj-batch4822";

/* ---- freestanding helpers ---------------------------------------------- */

/* High 32 bits of a packed (owner:seq) lock token. */
static uint32_t
b4822_owner(uint64_t u64Tok)
{
	return (uint32_t)(u64Tok >> 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lock_owner_u - extract owner (high 32) from a packed lock token.
 *
 * tok: packed uint64_t (high=owner, low=seq)
 *
 * Returns (uint32_t)(tok >> 32). Self-contained; no parent wires.
 */
uint32_t
gj_lock_owner_u(uint64_t u64Tok)
{
	(void)NULL;
	return b4822_owner(u64Tok);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lock_owner_u(uint64_t u64Tok)
    __attribute__((alias("gj_lock_owner_u")));
