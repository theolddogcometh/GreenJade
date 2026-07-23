/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4259: clear lowest set bit (u32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_clear_lsb_u(uint32_t x);
 *     - Return x & (x - 1): clear the least-significant 1-bit of x.
 *       Pure integer BLSR-like identity; no builtins.
 *       gj_u32_clear_lsb_u(0) == 0; gj_u32_clear_lsb_u(0x12) == 0x10.
 *   uint32_t __gj_u32_clear_lsb_u  (alias)
 *   __libcgj_batch4259_marker = "libcgj-batch4259"
 *
 * Exclusive continuum CREATE-ONLY (4251-4260). Unique
 * gj_u32_clear_lsb_u surface only; no multi-def. Distinct from
 * gj_u32_isolate_lsb_u (batch4258) which keeps the LSB. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4259_marker[] = "libcgj-batch4259";

/* ---- freestanding helpers ---------------------------------------------- */

/* BLSR / reset-lowest-set-bit: x & (x - 1). Zero stays zero. */
static uint32_t
b4259_clear_lsb(uint32_t u32X)
{
	return u32X & (u32X - 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_clear_lsb_u - clear the least-significant 1-bit of x.
 *
 * x: value to modify
 *
 * Returns x with its lowest set bit cleared (0 if x had at most one).
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_clear_lsb_u(uint32_t u32X)
{
	(void)NULL;
	return b4259_clear_lsb(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_clear_lsb_u(uint32_t u32X)
    __attribute__((alias("gj_u32_clear_lsb_u")));
