/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2544: non-atomic cooperative flag test-and-set.
 *
 * Surface (unique symbols):
 *   int gj_flag_test_and_set_u(volatile uint32_t *flag);
 *     - Sample *flag, then store set (1). Returns the previous value
 *       as 0 (was clear) or 1 (was already set). NULL flag -> 0.
 *       Plain volatile load then store; NOT an atomic TAS / BTS.
 *   int __gj_flag_test_and_set_u  (alias)
 *   __libcgj_batch2544_marker = "libcgj-batch2544"
 *
 * Convention (shared with batch2541-2543):
 *   *flag == 0 - clear / free
 *   *flag != 0 - set / held
 *
 * NON-ATOMIC COOPERATIVE SEMANTICS: the load and store are separate
 * plain volatile ops - not indivisible. Two cooperative callers can
 * both observe clear and both write set. For single-thread harnesses
 * or externally serialized ownership transfer only. Not C11
 * atomic_flag_test_and_set.
 *
 * Sync exclusive pure-volatile hints wave (2541-2550). Unique surface.
 * Distinct from gj_atomic_flag_test_and_set_hint (batch2152).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2544_marker[] = "libcgj-batch2544";

/* ---- freestanding helpers ---------------------------------------------- */

/* Normalize any nonzero prior value to 1 for the return convention. */
static int
b2544_as_bool(uint32_t u32V)
{
	if (u32V != 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flag_test_and_set_u - cooperative test-and-set.
 *
 * flag: pointer to volatile flag (0 clear / nonzero set); NULL -> 0.
 *
 * Returns previous state as 0 or 1, then forces *flag = 1.
 */
int
gj_flag_test_and_set_u(volatile uint32_t *pFlag)
{
	uint32_t u32Prev;
	int nWasSet;

	(void)NULL;

	if (pFlag == NULL) {
		return 0;
	}
	u32Prev = *pFlag;
	nWasSet = b2544_as_bool(u32Prev);
	*pFlag = 1u;
	return nWasSet;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_flag_test_and_set_u(volatile uint32_t *pFlag)
    __attribute__((alias("gj_flag_test_and_set_u")));
