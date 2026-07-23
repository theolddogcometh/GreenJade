/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1690: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1690(void);
 *     — Returns the compile-time graph batch number for this TU (1690).
 *   uint32_t __gj_batch_id_1690  (alias)
 *   __libcgj_batch1690_marker = "libcgj-batch1690"
 *
 * Milestone for the color / pixel exclusive integer wave
 * (batches 1681–1689). Does NOT redefine prior batch_id_* symbols —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1690_marker[] = "libcgj-batch1690";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1690 — report this TU's graph batch number.
 *
 * Always returns 1690. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1690(void)
{
	return 1690u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1690(void)
    __attribute__((alias("gj_batch_id_1690")));
