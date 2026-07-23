/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4829: lock wait deadline expiry predicate.
 *
 * Surface (unique symbols):
 *   int gj_lock_expired_u(uint64_t dl, uint64_t now);
 *     - Return 1 if now >= dl (deadline reached or passed), else 0.
 *   int __gj_lock_expired_u  (alias)
 *   __libcgj_batch4829_marker = "libcgj-batch4829"
 *
 * Exclusive continuum CREATE-ONLY (4821-4830: lock_token_u,
 * lock_owner_u, lock_seq_u, lock_is_free_u, lock_match_u,
 * lock_next_seq_u, lock_backoff_u, lock_deadline_u, lock_expired_u,
 * batch_id_4830). Unique gj_lock_expired_u surface only; no multi-def.
 * Distinct from gj_deadline_passed (batch1037; arg order now,deadline)
 * and gj_job_deadline_miss_p (batch1616). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4829_marker[] = "libcgj-batch4829";

/* ---- freestanding helpers ---------------------------------------------- */

/* Expired when current time has reached absolute deadline. */
static int
b4829_expired(uint64_t u64Dl, uint64_t u64Now)
{
	return (u64Now >= u64Dl) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lock_expired_u - 1 if lock wait deadline has expired.
 *
 * dl:  absolute deadline from gj_lock_deadline_u
 * now: current time (same unit as dl)
 *
 * Returns 1 when now >= dl, else 0. Arg order is (dl, now) — distinct
 * from gj_deadline_passed(now, deadline). Self-contained; no parent
 * wires.
 */
int
gj_lock_expired_u(uint64_t u64Dl, uint64_t u64Now)
{
	(void)NULL;
	return b4829_expired(u64Dl, u64Now);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_lock_expired_u(uint64_t u64Dl, uint64_t u64Now)
    __attribute__((alias("gj_lock_expired_u")));
