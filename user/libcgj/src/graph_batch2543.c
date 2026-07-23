/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2543: non-atomic cooperative flag test.
 *
 * Surface (unique symbols):
 *   int gj_flag_test_u(const volatile uint32_t *flag);
 *     - Sample *flag; return 1 if set (nonzero), 0 if clear.
 *       NULL flag -> 0. Plain volatile load; NOT an acquire atomic load.
 *   int __gj_flag_test_u  (alias)
 *   __libcgj_batch2543_marker = "libcgj-batch2543"
 *
 * Convention (shared with batch2541/2542/2544):
 *   *flag == 0 - clear / free
 *   *flag != 0 - set / held
 *
 * NON-ATOMIC COOPERATIVE SEMANTICS: pure volatile read. Compiler will
 * not cache across volatile, but there is no hardware acquire, no
 * LOCK, and no multi-core coherence guarantee beyond the platform
 * volatile model. Cooperative / single-thread or external sync only.
 *
 * Sync exclusive pure-volatile hints wave (2541-2550). Unique surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2543_marker[] = "libcgj-batch2543";

/* ---- freestanding helpers ---------------------------------------------- */

/* Normalize any nonzero value to 1 for the return convention. */
static int
b2543_as_bool(uint32_t u32V)
{
	if (u32V != 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flag_test_u - cooperative test of a volatile flag word.
 *
 * flag: pointer to volatile flag (0 clear / nonzero set); NULL -> 0.
 *
 * Returns 1 if the flag is set, 0 if clear or NULL.
 */
int
gj_flag_test_u(const volatile uint32_t *pFlag)
{
	(void)NULL;

	if (pFlag == NULL) {
		return 0;
	}
	return b2543_as_bool(*pFlag);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_flag_test_u(const volatile uint32_t *pFlag)
    __attribute__((alias("gj_flag_test_u")));
