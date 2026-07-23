/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch899: stream sample-count accessor.
 *
 * Surface (unique symbols):
 *   uint64_t gj_stream_count(const uint64_t *count);
 *     — Return *count, or 0 if count is NULL.
 *   uint64_t __gj_stream_count  (alias)
 *   __libcgj_batch899_marker = "libcgj-batch899"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch899_marker[] = "libcgj-batch899";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stream_count — read a stream's sample count from caller storage.
 *
 * NULL count → 0. Otherwise returns *count unchanged (no side effects).
 */
uint64_t
gj_stream_count(const uint64_t *count)
{
	if (count == NULL) {
		return (uint64_t)0;
	}
	return *count;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_stream_count(const uint64_t *count)
    __attribute__((alias("gj_stream_count")));
