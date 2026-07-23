/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1314: Barrier generation counter accessor.
 *
 * Surface (unique symbols):
 *   uint32_t gj_barrier_generation(const uint32_t *gen);
 *     — Return *gen. NULL → 0.
 *   uint32_t __gj_barrier_generation  (alias)
 *   __libcgj_batch1314_marker = "libcgj-batch1314"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1314_marker[] = "libcgj-batch1314";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_barrier_generation — read a barrier epoch/generation word.
 *
 * Callers bump generation when the last waiter arrives so late threads
 * can observe a new epoch. NULL → 0.
 */
uint32_t
gj_barrier_generation(const uint32_t *pGen)
{
	if (pGen == NULL) {
		return 0u;
	}
	return *pGen;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_barrier_generation(const uint32_t *pGen)
    __attribute__((alias("gj_barrier_generation")));

