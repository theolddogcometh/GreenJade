/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3138: remaining time until deadline (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_timeout_remaining_u(uint64_t now, uint64_t deadline);
 *     - Return deadline - now when now < deadline, else 0.
 *   uint64_t __gj_timeout_remaining_u  (alias)
 *   __libcgj_batch3138_marker = "libcgj-batch3138"
 *
 * Exclusive continuum CREATE-ONLY (3131-3140). Distinct from
 * gj_timeout_remaining (batch1038) — unique _u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3138_marker[] = "libcgj-batch3138";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3138_remaining(uint64_t u64Now, uint64_t u64Deadline)
{
	if (u64Now >= u64Deadline) {
		return 0ull;
	}
	return u64Deadline - u64Now;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timeout_remaining_u - time left until an absolute deadline.
 *
 * u64Now:      current time
 * u64Deadline: absolute deadline (same unit)
 *
 * Returns 0 if the deadline has already passed.
 */
uint64_t
gj_timeout_remaining_u(uint64_t u64Now, uint64_t u64Deadline)
{
	(void)NULL;
	return b3138_remaining(u64Now, u64Deadline);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_timeout_remaining_u(uint64_t u64Now, uint64_t u64Deadline)
    __attribute__((alias("gj_timeout_remaining_u")));
