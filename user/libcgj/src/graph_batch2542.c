/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2542: non-atomic cooperative flag clear.
 *
 * Surface (unique symbols):
 *   void gj_flag_clear_u(volatile uint32_t *flag);
 *     - Clear the flag word to free (0). NULL is a no-op.
 *       Plain volatile store; NOT a release barrier or atomic clear.
 *   void __gj_flag_clear_u  (alias)
 *   __libcgj_batch2542_marker = "libcgj-batch2542"
 *
 * Convention (shared with batch2541/2543/2544):
 *   *flag == 0 - clear / free
 *   *flag != 0 - set / held
 *
 * NON-ATOMIC COOPERATIVE SEMANTICS: does not validate ownership. No
 * fence. Cooperative / single-thread use only unless externally
 * serialized. Not C11 atomic_flag_clear.
 *
 * Sync exclusive pure-volatile hints wave (2541-2550). Unique surface.
 * Distinct from gj_atomic_flag_clear_hint (batch2153).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2542_marker[] = "libcgj-batch2542";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2542_clear_val(void)
{
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flag_clear_u - set flag word to clear (0).
 *
 * flag: pointer to caller-owned volatile word; NULL -> no-op.
 */
void
gj_flag_clear_u(volatile uint32_t *pFlag)
{
	(void)NULL;

	if (pFlag == NULL) {
		return;
	}
	*pFlag = b2542_clear_val();
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_flag_clear_u(volatile uint32_t *pFlag)
    __attribute__((alias("gj_flag_clear_u")));
