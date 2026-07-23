/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2541: non-atomic cooperative flag set.
 *
 * Surface (unique symbols):
 *   void gj_flag_set_u(volatile uint32_t *flag);
 *     - Set the flag word to held (1). NULL is a no-op.
 *       Plain volatile store; NOT an atomic store or release fence.
 *   void __gj_flag_set_u  (alias)
 *   __libcgj_batch2541_marker = "libcgj-batch2541"
 *
 * Convention (shared with batch2542-2544):
 *   *flag == 0 - clear / free
 *   *flag != 0 - set / held
 *
 * NON-ATOMIC COOPERATIVE SEMANTICS: pure volatile store only. No
 * __atomic_*, no LOCK prefix, no compiler or hardware fence. Safe for
 * single-thread / cooperative harnesses, or under an external serializer.
 * Racy under true concurrency - not a C11 atomic_flag.
 *
 * Sync exclusive pure-volatile hints wave (2541-2550). Unique surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2541_marker[] = "libcgj-batch2541";

/* ---- freestanding helpers ---------------------------------------------- */

/* Held flag word value (cooperative encoding). */
static uint32_t
b2541_set_val(void)
{
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flag_set_u - set flag word to held (1).
 *
 * flag: pointer to caller-owned volatile word; NULL -> no-op.
 */
void
gj_flag_set_u(volatile uint32_t *pFlag)
{
	(void)NULL;

	if (pFlag == NULL) {
		return;
	}
	*pFlag = b2541_set_val();
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_flag_set_u(volatile uint32_t *pFlag)
    __attribute__((alias("gj_flag_set_u")));
