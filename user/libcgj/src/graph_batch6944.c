/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6944: sendfile count clamp to remaining.
 *
 * Surface (unique symbols):
 *   uint64_t gj_sendfile_count_clamp_6944(uint64_t count, uint64_t remaining);
 *     - Return min(count, remaining) for a soft transfer length plan.
 *   uint64_t __gj_sendfile_count_clamp_6944  (alias)
 *   __libcgj_batch6944_marker = "libcgj-batch6944"
 *
 * Exclusive continuum CREATE-ONLY (6941-6950: sendfile offset stubs).
 * Unique gj_sendfile_count_clamp_6944 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6944_marker[] = "libcgj-batch6944";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b6944_clamp(uint64_t u64Count, uint64_t u64Remaining)
{
	if (u64Count > u64Remaining) {
		return u64Remaining;
	}
	return u64Count;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sendfile_count_clamp_6944 - clamp requested count to remaining.
 *
 * count:     soft requested transfer length
 * remaining: soft bytes available after the current offset
 *
 * Returns the smaller of count and remaining. Soft pure-data clamp;
 * no sendfile syscall. No parent wires.
 */
uint64_t
gj_sendfile_count_clamp_6944(uint64_t u64Count, uint64_t u64Remaining)
{
	(void)NULL;
	return b6944_clamp(u64Count, u64Remaining);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sendfile_count_clamp_6944(uint64_t u64Count, uint64_t u64Remaining)
    __attribute__((alias("gj_sendfile_count_clamp_6944")));
