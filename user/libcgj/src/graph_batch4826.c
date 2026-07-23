/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4826: advance lock sequence counter.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lock_next_seq_u(uint32_t seq);
 *     - Return seq + 1 with natural uint32_t wrap. Used when packing
 *       a new lock token after acquire/release.
 *   uint32_t __gj_lock_next_seq_u  (alias)
 *   __libcgj_batch4826_marker = "libcgj-batch4826"
 *
 * Exclusive continuum CREATE-ONLY (4821-4830: lock_token_u,
 * lock_owner_u, lock_seq_u, lock_is_free_u, lock_match_u,
 * lock_next_seq_u, lock_backoff_u, lock_deadline_u, lock_expired_u,
 * batch_id_4830). Unique gj_lock_next_seq_u surface only; no multi-def.
 * Distinct from gj_slot_alloc_gen_u (batch4763). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4826_marker[] = "libcgj-batch4826";

/* ---- freestanding helpers ---------------------------------------------- */

/* Next sequence: wrap-safe uint32 increment. */
static uint32_t
b4826_next_seq(uint32_t u32Seq)
{
	return u32Seq + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lock_next_seq_u - advance a lock sequence counter by one.
 *
 * seq: current low-32 sequence field
 *
 * Returns seq + 1 (wraps through UINT32_MAX to 0). Self-contained;
 * no parent wires.
 */
uint32_t
gj_lock_next_seq_u(uint32_t u32Seq)
{
	(void)NULL;
	return b4826_next_seq(u32Seq);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lock_next_seq_u(uint32_t u32Seq)
    __attribute__((alias("gj_lock_next_seq_u")));
