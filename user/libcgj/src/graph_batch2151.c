/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2151: non-atomic atomic_flag init hint.
 *
 * Surface (unique symbols):
 *   void gj_atomic_flag_init_hint(volatile uint32_t *flag);
 *     - Initialize a cooperative flag word to clear (0).
 *       NULL is a no-op. Plain volatile store; NOT stdatomic.
 *   void __gj_atomic_flag_init_hint  (alias)
 *   __libcgj_batch2151_marker = "libcgj-batch2151"
 *
 * Convention (shared with batch2152/2153):
 *   *flag == 0 - clear / free
 *   *flag != 0 - set / held
 *
 * NON-ATOMIC COOPERATIVE SEMANTICS: pure volatile load/store only. No
 * __atomic_*, no LOCK prefix, no compiler or hardware fence. Safe for
 * single-thread / cooperative harnesses, or under an external serializer.
 * Racy under true concurrency - not a C11 atomic_flag.
 *
 * Post-2150 atomics-hint exclusive wave (pure volatile). Unique surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2151_marker[] = "libcgj-batch2151";

/* ---- freestanding helpers ---------------------------------------------- */

/* Cleared flag word value (cooperative encoding). */
static uint32_t
b2151_clear_val(void)
{
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_flag_init_hint - set flag word to clear (0).
 *
 * flag: pointer to caller-owned volatile word; NULL -> no-op.
 */
void
gj_atomic_flag_init_hint(volatile uint32_t *pFlag)
{
	(void)NULL;

	if (pFlag == NULL) {
		return;
	}
	*pFlag = b2151_clear_val();
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_atomic_flag_init_hint(volatile uint32_t *pFlag)
    __attribute__((alias("gj_atomic_flag_init_hint")));
