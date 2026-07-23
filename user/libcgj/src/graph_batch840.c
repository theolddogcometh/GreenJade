/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch840: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_840(void);
 *     — Returns the compile-time graph batch number for this TU (840).
 *   uint32_t __gj_batch_id_840  (alias)
 *   __libcgj_batch840_marker = "libcgj-batch840"
 *
 * Does NOT define gj_batch_id / gj_batch_id_830 / prior batch_id_* or
 * milestone_* surfaces — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch840_marker[] = "libcgj-batch840";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_840 — report this TU's graph batch number.
 *
 * Always returns 840.
 */
uint32_t
gj_batch_id_840(void)
{
	return 840u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_840(void)
    __attribute__((alias("gj_batch_id_840")));
