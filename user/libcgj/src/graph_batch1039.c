/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1039: monotonic clock delta.
 *
 * Surface (unique symbols):
 *   uint64_t gj_monotonic_delta(uint64_t start, uint64_t now);
 *     — Return now - start when now >= start, else 0 (defends against
 *       non-monotonic glitches without wrapping).
 *   uint64_t __gj_monotonic_delta  (alias)
 *   __libcgj_batch1039_marker = "libcgj-batch1039"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1039_marker[] = "libcgj-batch1039";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_monotonic_delta — elapsed time on a monotonic clock.
 *
 * u64Start: start timestamp
 * u64Now:   current timestamp (same unit)
 *
 * Returns 0 if now appears earlier than start (no wrap-around delta).
 */
uint64_t
gj_monotonic_delta(uint64_t u64Start, uint64_t u64Now)
{
	if (u64Now < u64Start) {
		return 0u;
	}
	return u64Now - u64Start;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_monotonic_delta(uint64_t u64Start, uint64_t u64Now)
    __attribute__((alias("gj_monotonic_delta")));
