/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1680: milestone 1680 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1680(void);
 *     — Returns the compile-time graph batch number for this TU (1680).
 *       Milestone for the audio/sample exclusive integer wave
 *       (batches 1671–1679: PCM clip/convert/mix/interleave/RMS/rate/
 *       frame-bytes).
 *   uint32_t __gj_batch_id_1680  (alias)
 *   __libcgj_batch1680_marker = "libcgj-batch1680"
 *
 * Does NOT redefine prior batch_id / milestone symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1680_marker[] = "libcgj-batch1680";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1680 — report this TU's graph batch number.
 *
 * Always returns 1680.
 */
uint32_t
gj_batch_id_1680(void)
{
	return 1680u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1680(void)
    __attribute__((alias("gj_batch_id_1680")));
