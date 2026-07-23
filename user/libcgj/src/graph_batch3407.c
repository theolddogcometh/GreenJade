/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3407: u32 BLSR (reset lowest set bit).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_blsr_u(uint32_t x);
 *     - Return x & (x - 1): clear the least-significant 1-bit of x.
 *       Pure unsigned arithmetic; no builtins.
 *   uint32_t __gj_u32_blsr_u  (alias)
 *   __libcgj_batch3407_marker = "libcgj-batch3407"
 *
 * Exclusive continuum CREATE-ONLY (3401-3410). Distinct from
 * gj_u32_blsi_u (batch3405) — unique gj_u32_blsr_u surface only; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3407_marker[] = "libcgj-batch3407";

/* ---- freestanding helpers ---------------------------------------------- */

/* BMI1 BLSR: reset lowest set bit via x & (x - 1). */
static uint32_t
b3407_blsr(uint32_t uX)
{
	return uX & (uX - 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_blsr_u - clear the least-significant 1-bit of x.
 *
 * x: unsigned 32-bit value
 *
 * Identity: x & (x - 1). Examples: 0 → 0; 0x10 → 0; 0x12 → 0x10;
 * 0xffffffffu → 0xfffffffeu. No parent wires.
 */
uint32_t
gj_u32_blsr_u(uint32_t uX)
{
	(void)NULL;
	return b3407_blsr(uX);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_blsr_u(uint32_t uX)
    __attribute__((alias("gj_u32_blsr_u")));
