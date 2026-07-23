/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2299: product bar3 readiness soft hint.
 *
 * Surface (unique symbols):
 *   int gj_bar3_ready_hint(uint32_t mask);
 *     - Return 1 if mask has all three product bar3 readiness bits
 *       set (bit0, bit1, bit2), i.e. (mask & 0x7) == 0x7. Else 0.
 *       Soft pure-data readiness hint for the third product bar wave;
 *       not a runtime capability probe.
 *   int __gj_bar3_ready_hint  (alias)
 *   __libcgj_batch2299_marker = "libcgj-batch2299"
 *
 * Milestone 2300 exclusive product helpers (2291-2300). Unique
 * gj_bar3_ready_hint surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2299_marker[] = "libcgj-batch2299";

/* Three product bar3 readiness bits (bits 0..2). */
#define B2299_BAR3_MASK  0x7u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if all bar3 readiness bits are present in mask. */
static int
b2299_ready(uint32_t uMask)
{
	if ((uMask & B2299_BAR3_MASK) == B2299_BAR3_MASK) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_ready_hint - soft check that bar3 readiness mask is complete.
 *
 * mask: product bar3 readiness bitmask (low three bits required)
 *
 * Returns 1 when (mask & 0x7) == 0x7, else 0. Extra high bits are
 * ignored. Does not call libc.
 */
int
gj_bar3_ready_hint(uint32_t uMask)
{
	(void)NULL;
	return b2299_ready(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bar3_ready_hint(uint32_t uMask)
    __attribute__((alias("gj_bar3_ready_hint")));
