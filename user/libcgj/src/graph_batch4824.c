/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4824: lock token free predicate (tok == 0).
 *
 * Surface (unique symbols):
 *   int gj_lock_is_free_u(uint64_t tok);
 *     - Return 1 if tok == 0 (unlocked / free), else 0.
 *   int __gj_lock_is_free_u  (alias)
 *   __libcgj_batch4824_marker = "libcgj-batch4824"
 *
 * Exclusive continuum CREATE-ONLY (4821-4830: lock_token_u,
 * lock_owner_u, lock_seq_u, lock_is_free_u, lock_match_u,
 * lock_next_seq_u, lock_backoff_u, lock_deadline_u, lock_expired_u,
 * batch_id_4830). Unique gj_lock_is_free_u surface only; no multi-def.
 * Distinct from gj_slot_is_free_u (batch4761). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4824_marker[] = "libcgj-batch4824";

/* ---- freestanding helpers ---------------------------------------------- */

/* Free iff the entire packed token is zero. */
static int
b4824_is_free(uint64_t u64Tok)
{
	return (u64Tok == 0ULL) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lock_is_free_u - 1 if lock token is free (tok == 0).
 *
 * tok: packed uint64_t (high=owner, low=seq)
 *
 * Returns 1 when tok == 0, else 0. Self-contained; no parent wires.
 */
int
gj_lock_is_free_u(uint64_t u64Tok)
{
	(void)NULL;
	return b4824_is_free(u64Tok);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_lock_is_free_u(uint64_t u64Tok)
    __attribute__((alias("gj_lock_is_free_u")));
