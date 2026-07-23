/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1560: milestone 1560 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1560(void);
 *     — Returns the compile-time graph batch number for this TU (1560).
 *   uint32_t __gj_batch_id_1560  (alias)
 *   __libcgj_batch1560_marker = "libcgj-batch1560"
 *
 * Milestone for the sync-primitives-hints exclusive theme
 * (batch1551–1559). Does NOT redefine prior batch_id_* symbols.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1560_marker[] = "libcgj-batch1560";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1560 — report this TU's graph batch number.
 *
 * Always returns 1560.
 */
uint32_t
gj_batch_id_1560(void)
{
	return 1560u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1560(void)
    __attribute__((alias("gj_batch_id_1560")));
