/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8259: test-bit integer stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bit_test_u_8259(uint32_t v, uint32_t i);
 *     - Return 1 if bit i of v is set (i in 0..31, LSB = bit 0), else 0.
 *       i >= 32 → 0.
 *   uint32_t __gj_bit_test_u_8259  (alias)
 *   __libcgj_batch8259_marker = "libcgj-batch8259"
 *
 * Exclusive continuum CREATE-ONLY (8251-8260: bitops / ffs-style integer
 * stubs — ffs, clz, ctz, popcount, parity, isolate_lsb, clear_lsb, mask,
 * test, batch_id_8260). Unique gj_bit_test_u_8259 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8259_marker[] = "libcgj-batch8259";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8259_test — 1 if bit i of v is set; 0 if clear or i out of range.
 */
static uint32_t
b8259_test(uint32_t u32V, uint32_t u32I)
{
	if (u32I >= 32u) {
		return 0u;
	}
	return ((u32V >> u32I) & 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_test_u_8259 - test whether bit i of v is set.
 *
 * v: word to inspect
 * i: bit index (0 = LSB, 31 = MSB); i >= 32 → not set
 *
 * Returns 1 when bit i is 1, else 0. Soft bit-test catalog surface;
 * pure integer. No parent wires.
 */
uint32_t
gj_bit_test_u_8259(uint32_t u32V, uint32_t u32I)
{
	(void)NULL;
	return b8259_test(u32V, u32I);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bit_test_u_8259(uint32_t u32V, uint32_t u32I)
    __attribute__((alias("gj_bit_test_u_8259")));
