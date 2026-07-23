/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4827: lock spin backoff delay units.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lock_backoff_u(uint32_t attempt);
 *     - Exponential spin units: 1 << attempt, capped at 65536 when
 *       attempt >= 16. attempt is 0-based (0 → 1 unit).
 *   uint32_t __gj_lock_backoff_u  (alias)
 *   __libcgj_batch4827_marker = "libcgj-batch4827"
 *
 * Exclusive continuum CREATE-ONLY (4821-4830: lock_token_u,
 * lock_owner_u, lock_seq_u, lock_is_free_u, lock_match_u,
 * lock_next_seq_u, lock_backoff_u, lock_deadline_u, lock_expired_u,
 * batch_id_4830). Unique gj_lock_backoff_u surface only; no multi-def.
 * Distinct from gj_retry_backoff_us (batch1195) and gj_backoff_exp_ms
 * (batch1979). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4827_marker[] = "libcgj-batch4827";

/* Cap shift so 1 << attempt stays within uint32 and a useful spin bound. */
#define B4827_BACKOFF_SHIFT_CAP  16u
#define B4827_BACKOFF_MAX        65536u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Exponential spin units: 2^attempt, hard-capped at 2^16.
 * attempt 0 → 1; attempt 15 → 32768; attempt >= 16 → 65536.
 */
static uint32_t
b4827_backoff(uint32_t u32Attempt)
{
	if (u32Attempt >= B4827_BACKOFF_SHIFT_CAP) {
		return B4827_BACKOFF_MAX;
	}
	return 1u << u32Attempt;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lock_backoff_u - exponential lock spin backoff unit count.
 *
 * attempt: 0-based retry index
 *
 * Returns 1 << attempt when attempt < 16, else 65536. Units are
 * caller-defined (spins / pauses). Self-contained; no parent wires.
 */
uint32_t
gj_lock_backoff_u(uint32_t u32Attempt)
{
	(void)NULL;
	return b4827_backoff(u32Attempt);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lock_backoff_u(uint32_t u32Attempt)
    __attribute__((alias("gj_lock_backoff_u")));
