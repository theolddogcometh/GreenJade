/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1440: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1440(void);
 *     — Returns the compile-time graph batch number for this TU (1440).
 *   uint32_t __gj_batch_id_1440  (alias)
 *   __libcgj_batch1440_marker = "libcgj-batch1440"
 *
 * Does NOT redefine gj_batch_id (batch500) or other gj_batch_id_N
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1440_marker[] = "libcgj-batch1440";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1440 — report this TU's graph batch number.
 *
 * Always returns 1440. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1440(void)
{
	return 1440u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1440(void)
    __attribute__((alias("gj_batch_id_1440")));
