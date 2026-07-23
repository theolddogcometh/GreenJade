/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2152: non-atomic atomic_flag test-and-set hint.
 *
 * Surface (unique symbols):
 *   int gj_atomic_flag_test_and_set_hint(volatile uint32_t *flag);
 *     - Sample *flag, then store set (1). Returns the previous value
 *       as 0 (was clear) or 1 (was already set). NULL flag -> 0.
 *       Plain volatile load then store; NOT an atomic TAS / BTS.
 *   int __gj_atomic_flag_test_and_set_hint  (alias)
 *   __libcgj_batch2152_marker = "libcgj-batch2152"
 *
 * Convention (shared with batch2151/2153):
 *   *flag == 0 - clear / free
 *   *flag != 0 - set / held
 *
 * NON-ATOMIC COOPERATIVE SEMANTICS: the load and store are separate
 * plain volatile ops - not indivisible. Two cooperative callers can
 * both observe clear and both write set. For single-thread harnesses
 * or externally serialized ownership transfer only. Not C11
 * atomic_flag_test_and_set.
 *
 * Post-2150 atomics-hint exclusive wave (pure volatile). Unique surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2152_marker[] = "libcgj-batch2152";

/* ---- freestanding helpers ---------------------------------------------- */

/* Normalize any nonzero prior value to 1 for the return convention. */
static int
b2152_as_bool(uint32_t u32V)
{
	if (u32V != 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_flag_test_and_set_hint - cooperative test-and-set.
 *
 * flag: pointer to volatile flag (0 clear / nonzero set); NULL -> 0.
 *
 * Returns previous state as 0 or 1, then forces *flag = 1.
 */
int
gj_atomic_flag_test_and_set_hint(volatile uint32_t *pFlag)
{
	uint32_t u32Prev;
	int nWasSet;

	(void)NULL;

	if (pFlag == NULL) {
		return 0;
	}
	u32Prev = *pFlag;
	nWasSet = b2152_as_bool(u32Prev);
	*pFlag = 1u;
	return nWasSet;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_atomic_flag_test_and_set_hint(volatile uint32_t *pFlag)
    __attribute__((alias("gj_atomic_flag_test_and_set_hint")));
