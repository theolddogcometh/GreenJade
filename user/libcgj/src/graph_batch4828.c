/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4828: absolute lock wait deadline.
 *
 * Surface (unique symbols):
 *   uint64_t gj_lock_deadline_u(uint64_t now, uint64_t to);
 *     - Return now + to with saturating add (overflow → UINT64_MAX).
 *       Units are caller-defined (typically monotonic ticks/ns).
 *   uint64_t __gj_lock_deadline_u  (alias)
 *   __libcgj_batch4828_marker = "libcgj-batch4828"
 *
 * Exclusive continuum CREATE-ONLY (4821-4830: lock_token_u,
 * lock_owner_u, lock_seq_u, lock_is_free_u, lock_match_u,
 * lock_next_seq_u, lock_backoff_u, lock_deadline_u, lock_expired_u,
 * batch_id_4830). Unique gj_lock_deadline_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4828_marker[] = "libcgj-batch4828";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating now + timeout → absolute deadline. */
static uint64_t
b4828_deadline(uint64_t u64Now, uint64_t u64To)
{
	if (u64To > (UINT64_MAX - u64Now)) {
		return UINT64_MAX;
	}
	return u64Now + u64To;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lock_deadline_u - compute absolute lock wait deadline.
 *
 * now: current time (same unit as to)
 * to:  relative timeout duration
 *
 * Returns now + to, or UINT64_MAX on overflow. Self-contained; no
 * parent wires.
 */
uint64_t
gj_lock_deadline_u(uint64_t u64Now, uint64_t u64To)
{
	(void)NULL;
	return b4828_deadline(u64Now, u64To);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_lock_deadline_u(uint64_t u64Now, uint64_t u64To)
    __attribute__((alias("gj_lock_deadline_u")));
