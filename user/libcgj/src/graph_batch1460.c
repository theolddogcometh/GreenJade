/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1460: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1460(void);
 *     — Returns the compile-time graph batch number for this TU (1460).
 *   uint32_t __gj_batch_id_1460  (alias)
 *   __libcgj_batch1460_marker = "libcgj-batch1460"
 *
 * Milestone for the mat2/vec2 i32 helper continuum (batches 1451–1459).
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1460_marker[] = "libcgj-batch1460";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1460 — report this TU's graph batch number.
 *
 * Always returns 1460.
 */
uint32_t
gj_batch_id_1460(void)
{
	return 1460u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1460(void)
    __attribute__((alias("gj_batch_id_1460")));
