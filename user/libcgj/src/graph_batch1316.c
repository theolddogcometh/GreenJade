/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1316: Countdown latch done predicate.
 *
 * Surface (unique symbols):
 *   int gj_latch_is_done(uint32_t remaining);
 *     — Return 1 if remaining == 0, else 0.
 *   int __gj_latch_is_done  (alias)
 *   __libcgj_batch1316_marker = "libcgj-batch1316"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stdint.h>

const char __libcgj_batch1316_marker[] = "libcgj-batch1316";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_latch_is_done — 1 when a countdown latch has reached zero.
 */
int
gj_latch_is_done(uint32_t uRemaining)
{
	return (uRemaining == 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_latch_is_done(uint32_t uRemaining)
    __attribute__((alias("gj_latch_is_done")));

