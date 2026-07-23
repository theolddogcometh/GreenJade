/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6946: sendfile planned transfer count.
 *
 * Surface (unique symbols):
 *   uint64_t gj_sendfile_plan_count_6946(uint64_t size, uint64_t offset,
 *                                       uint64_t count);
 *     - Clamp count to bytes remaining after offset in a file of size.
 *   uint64_t __gj_sendfile_plan_count_6946  (alias)
 *   __libcgj_batch6946_marker = "libcgj-batch6946"
 *
 * Exclusive continuum CREATE-ONLY (6941-6950: sendfile offset stubs).
 * Unique gj_sendfile_plan_count_6946 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6946_marker[] = "libcgj-batch6946";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b6946_plan(uint64_t u64Size, uint64_t u64Offset, uint64_t u64Count)
{
	uint64_t u64Rem;

	if (u64Offset >= u64Size) {
		return 0ull;
	}
	u64Rem = u64Size - u64Offset;
	if (u64Count > u64Rem) {
		return u64Rem;
	}
	return u64Count;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sendfile_plan_count_6946 - planned transfer length from size/offset.
 *
 * size:   soft in_fd file size
 * offset: soft sendfile file offset
 * count:  soft requested transfer length
 *
 * Returns min(count, remaining) where remaining is 0 when
 * offset >= size. Soft pure-data transfer planner; no sendfile.
 * No parent wires.
 */
uint64_t
gj_sendfile_plan_count_6946(uint64_t u64Size, uint64_t u64Offset,
    uint64_t u64Count)
{
	(void)NULL;
	return b6946_plan(u64Size, u64Offset, u64Count);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sendfile_plan_count_6946(uint64_t u64Size, uint64_t u64Offset,
    uint64_t u64Count)
    __attribute__((alias("gj_sendfile_plan_count_6946")));
