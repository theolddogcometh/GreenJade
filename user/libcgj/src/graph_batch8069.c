/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8069: memcmp order unsigned-octet asc stub.
 *
 * Surface (unique symbols):
 *   int gj_memcmp_order_u8_asc_8069(uint32_t a, uint32_t b);
 *     - Compare low 8 bits of a,b as unsigned octets; -1 / 0 / 1.
 *   int __gj_memcmp_order_u8_asc_8069  (alias)
 *   __libcgj_batch8069_marker = "libcgj-batch8069"
 *
 * Exclusive continuum CREATE-ONLY (8061-8070: memcmp order stubs —
 * eq_id, lt_id, gt_id, is_eq, is_lt, is_gt, is_ne, sign_norm,
 * u8_asc, continuum + batch_id_8070). Unique
 * gj_memcmp_order_u8_asc_8069 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8069_marker[] = "libcgj-batch8069";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b8069_u8_asc(uint32_t u32A, uint32_t u32B)
{
	uint32_t u8A;
	uint32_t u8B;

	u8A = u32A & 0xFFu;
	u8B = u32B & 0xFFu;
	if (u8A < u8B)
		return -1;
	if (u8A > u8B)
		return 1;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memcmp_order_u8_asc_8069 - single-octet ascending order like memcmp.
 *
 * a: left value (only low 8 bits used)
 * b: right value (only low 8 bits used)
 *
 * Returns -1 / 0 / 1 for unsigned-char lexicographic order. Soft pure
 * byte compare stub; does not call libc memcmp. No parent wires.
 */
int
gj_memcmp_order_u8_asc_8069(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8069_u8_asc(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_memcmp_order_u8_asc_8069(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_memcmp_order_u8_asc_8069")));
