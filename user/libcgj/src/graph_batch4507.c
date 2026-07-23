/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4507: pack four boolean values into low bits.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_pack_bool4_u(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
 *     - Collapse each of a,b,c,d to a single 0/1 bit (nonzero -> 1) and
 *       place them at bits 0,1,2,3 respectively. Higher bits of the
 *       result are zero.
 *   uint32_t __gj_u32_pack_bool4_u  (alias)
 *   __libcgj_batch4507_marker = "libcgj-batch4507"
 *
 * Exclusive continuum CREATE-ONLY (4501-4510). Distinct from
 * gj_unpack_bools8 (batch933) — unique gj_u32_pack_bool4_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4507_marker[] = "libcgj-batch4507";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4507_pack_bool4(uint32_t uA, uint32_t uB, uint32_t uC, uint32_t uD)
{
	uint32_t uOut = 0u;

	if (uA != 0u) {
		uOut |= 1u << 0;
	}
	if (uB != 0u) {
		uOut |= 1u << 1;
	}
	if (uC != 0u) {
		uOut |= 1u << 2;
	}
	if (uD != 0u) {
		uOut |= 1u << 3;
	}
	return uOut;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_pack_bool4_u - pack four truthy/falsey words into bits 0..3.
 *
 * a,b,c,d: values treated as bool (nonzero -> 1, zero -> 0)
 *
 * Returns a 4-bit field in the low nibble: bit0=a, bit1=b, bit2=c, bit3=d.
 * Example: gj_u32_pack_bool4_u(1,0,1,1) -> 0xDu.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_pack_bool4_u(uint32_t uA, uint32_t uB, uint32_t uC, uint32_t uD)
{
	(void)NULL;
	return b4507_pack_bool4(uA, uB, uC, uD);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_pack_bool4_u(uint32_t uA, uint32_t uB, uint32_t uC, uint32_t uD)
    __attribute__((alias("gj_u32_pack_bool4_u")));
