/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1340: batch identity marker.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1340(void);
 *     — Returns the compile-time graph batch number for this TU (1340).
 *   uint32_t __gj_batch_id_1340  (alias)
 *   __libcgj_batch1340_marker = "libcgj-batch1340"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1340_marker[] = "libcgj-batch1340";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1340 — report this TU's graph batch number.
 *
 * Always returns 1340.
 */
uint32_t
gj_batch_id_1340(void)
{
	return 1340u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1340(void)
    __attribute__((alias("gj_batch_id_1340")));
