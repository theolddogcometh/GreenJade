/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5918: product smoke/dyn gate pair extract.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_gate_smoke_bit_5918(uint32_t packed);
 *     - Extract bit 0 (smoke) from a product gate pack word (batch5917
 *       layout). Returns 0 or 1.
 *   uint32_t gj_product_gate_dyn_bit_5918(uint32_t packed);
 *     - Extract bit 1 (dyn) from a product gate pack word. Returns 0
 *       or 1.
 *   uint32_t __gj_product_gate_smoke_bit_5918  (alias)
 *   uint32_t __gj_product_gate_dyn_bit_5918  (alias)
 *   __libcgj_batch5918_marker = "libcgj-batch5918"
 *
 * Exclusive continuum CREATE-ONLY (5911-5920: smoke/dyn gate mirror
 * helpers for product). Unique *_5918 surfaces only; no multi-def.
 * Pair with gj_product_gate_pack_5917. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5918_marker[] = "libcgj-batch5918";

#define B5918_SMOKE_BIT  0x1u
#define B5918_DYN_BIT    0x2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5918_smoke(uint32_t u32Packed)
{
	return (u32Packed & B5918_SMOKE_BIT) != 0u ? 1u : 0u;
}

static uint32_t
b5918_dyn(uint32_t u32Packed)
{
	return (u32Packed & B5918_DYN_BIT) != 0u ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_gate_smoke_bit_5918 - extract smoke bit from pack word.
 *
 * packed: product gate pack from batch5917 (or compatible layout)
 *
 * Returns 1 if bit 0 set, else 0. Soft pure-data. No parent wires.
 */
uint32_t
gj_product_gate_smoke_bit_5918(uint32_t packed)
{
	(void)NULL;
	return b5918_smoke(packed);
}

/*
 * gj_product_gate_dyn_bit_5918 - extract dyn bit from pack word.
 *
 * packed: product gate pack from batch5917 (or compatible layout)
 *
 * Returns 1 if bit 1 set, else 0. Soft pure-data. No parent wires.
 */
uint32_t
gj_product_gate_dyn_bit_5918(uint32_t packed)
{
	return b5918_dyn(packed);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_product_gate_smoke_bit_5918(uint32_t packed)
    __attribute__((alias("gj_product_gate_smoke_bit_5918")));

uint32_t __gj_product_gate_dyn_bit_5918(uint32_t packed)
    __attribute__((alias("gj_product_gate_dyn_bit_5918")));
