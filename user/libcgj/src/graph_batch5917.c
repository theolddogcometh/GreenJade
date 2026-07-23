/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5917: product smoke/dyn gate pair pack.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_gate_pack_5917(uint32_t smoke, uint32_t dyn);
 *     - Pack normalized smoke (bit 0) and dyn (bit 1) into a soft
 *       product gate pair word. Higher bits are zero. Soft pure-data;
 *       does not store a table.
 *   uint32_t __gj_product_gate_pack_5917  (alias)
 *   __libcgj_batch5917_marker = "libcgj-batch5917"
 *
 * Exclusive continuum CREATE-ONLY (5911-5920: smoke/dyn gate mirror
 * helpers for product). Unique gj_product_gate_pack_5917 surface only;
 * no multi-def. Pair with extract helpers in batch5918. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5917_marker[] = "libcgj-batch5917";

#define B5917_SMOKE_BIT  0x1u
#define B5917_DYN_BIT    0x2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5917_norm01(uint32_t u32V)
{
	return (u32V != 0u) ? 1u : 0u;
}

static uint32_t
b5917_pack(uint32_t u32Smoke, uint32_t u32Dyn)
{
	uint32_t u32P = 0u;

	if (b5917_norm01(u32Smoke) != 0u) {
		u32P |= B5917_SMOKE_BIT;
	}
	if (b5917_norm01(u32Dyn) != 0u) {
		u32P |= B5917_DYN_BIT;
	}
	return u32P;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_gate_pack_5917 - pack smoke/dyn into a product gate word.
 *
 * smoke: soft smoke-gate lamp → bit 0
 * dyn:   soft dyn-gate lamp   → bit 1
 *
 * Returns packed 2-bit word in [0, 3]. Soft pure-data product mirror.
 * No parent wires.
 */
uint32_t
gj_product_gate_pack_5917(uint32_t smoke, uint32_t dyn)
{
	(void)NULL;
	return b5917_pack(smoke, dyn);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_gate_pack_5917(uint32_t smoke, uint32_t dyn)
    __attribute__((alias("gj_product_gate_pack_5917")));
