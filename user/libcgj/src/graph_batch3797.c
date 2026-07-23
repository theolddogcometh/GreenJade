/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3797: product bar3 readiness gate (wave 3800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_ready_3800(uint32_t mask);
 *     - Return 1 if mask has all three product bar3 readiness bits
 *       set (bit0, bit1, bit2), i.e. (mask & 0x7) == 0x7. Else 0.
 *       Soft pure-data readiness gate for Steam/Deck/Top50 on the
 *       milestone 3800 continuum.
 *   uint32_t __gj_bar3_ready_3800  (alias)
 *   __libcgj_batch3797_marker = "libcgj-batch3797"
 *
 * Milestone 3800 exclusive continuum CREATE-ONLY (3791-3800). Unique
 * gj_bar3_ready_3800 surface only; no multi-def. Distinct from
 * gj_bar3_ready_3700 (batch3697), gj_bar3_ready_3600 (batch3597),
 * gj_bar3_ready_3550 (batch3547), and gj_bar3_ready_hint
 * (batch2299). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3797_marker[] = "libcgj-batch3797";

/* Three product bar3 readiness bits (bits 0..2). */
#define B3797_BAR3_MASK  0x7u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if all bar3 readiness bits are present in mask. */
static uint32_t
b3797_ready(uint32_t uMask)
{
	if ((uMask & B3797_BAR3_MASK) == B3797_BAR3_MASK) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_ready_3800 - 1u iff bar3 readiness mask is complete.
 *
 * mask: product bar3 readiness bitmask (bits 0..2 = Steam/Deck/Top50)
 *
 * Returns 1 when (mask & 0x7) == 0x7, else 0. Extra high bits are
 * ignored. Soft pure-data only; does not probe Steam/Deck. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_bar3_ready_3800(uint32_t uMask)
{
	(void)NULL;
	return b3797_ready(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_ready_3800(uint32_t uMask)
    __attribute__((alias("gj_bar3_ready_3800")));
