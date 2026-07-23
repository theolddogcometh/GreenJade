/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1315: Barrier wait-count accessor.
 *
 * Surface (unique symbols):
 *   uint32_t gj_barrier_wait_count(const uint32_t *count);
 *     — Return *count (threads currently waiting). NULL → 0.
 *   uint32_t __gj_barrier_wait_count  (alias)
 *   __libcgj_batch1315_marker = "libcgj-batch1315"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1315_marker[] = "libcgj-batch1315";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_barrier_wait_count — read how many threads are waiting at a barrier.
 *
 * NULL → 0.
 */
uint32_t
gj_barrier_wait_count(const uint32_t *pCount)
{
	if (pCount == NULL) {
		return 0u;
	}
	return *pCount;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_barrier_wait_count(const uint32_t *pCount)
    __attribute__((alias("gj_barrier_wait_count")));

