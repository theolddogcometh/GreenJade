/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1490: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1490(void);
 *     — Returns the compile-time graph batch number for this TU (1490).
 *   uint32_t __gj_batch_id_1490  (alias)
 *   __libcgj_batch1490_marker = "libcgj-batch1490"
 *
 * Does NOT redefine gj_batch_id / prior gj_batch_id_* — avoid multi-def.
 * Milestone for the UTF-8 / Unicode scalar exclusive continuum 1481..1490.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1490_marker[] = "libcgj-batch1490";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1490 — report this TU's graph batch number.
 *
 * Always returns 1490.
 */
uint32_t
gj_batch_id_1490(void)
{
	return 1490u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1490(void)
    __attribute__((alias("gj_batch_id_1490")));
