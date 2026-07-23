/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1732: 128-bit × u64 multiply with high carry hint.
 *
 * Surface (unique symbols):
 *   int gj_u128_mul_u64_hint(uint64_t *lo, uint64_t *hi, uint64_t m,
 *                            uint64_t *carry_out);
 *     — Treat (*lo, *hi) as little-endian 128-bit. Multiply by m in place.
 *       Product is 192-bit: *lo / *hi receive the low 128 bits; optional
 *       *carry_out receives the top 64 bits. NULL carry_out skips top.
 *       lo/hi NULL → -1. Returns 0 on success.
 *   int __gj_u128_mul_u64_hint  (alias)
 *   __libcgj_batch1732_marker = "libcgj-batch1732"
 *
 * Decimal/fixed-print exclusive wave. Portable 32-bit partial products
 * (no __int128). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1732_marker[] = "libcgj-batch1732";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1732_umul64x64 — portable 64×64 → 128 (lo, hi out).
 */
static void
b1732_umul64x64(uint64_t u64A, uint64_t u64B, uint64_t *pLo, uint64_t *pHi)
{
	uint64_t u64Al;
	uint64_t u64Ah;
	uint64_t u64Bl;
	uint64_t u64Bh;
	uint64_t u64Ll;
	uint64_t u64Hl;
	uint64_t u64Lh;
	uint64_t u64Hh;
	uint64_t u64Mid;
	uint64_t u64Upper;
	uint64_t u64Lower;

	u64Al = u64A & 0xffffffffull;
	u64Ah = u64A >> 32;
	u64Bl = u64B & 0xffffffffull;
	u64Bh = u64B >> 32;

	u64Ll = u64Al * u64Bl;
	u64Hl = u64Ah * u64Bl;
	u64Lh = u64Al * u64Bh;
	u64Hh = u64Ah * u64Bh;

	u64Mid = (u64Ll >> 32) + (u64Hl & 0xffffffffull) + u64Lh;
	u64Upper = u64Hh + (u64Hl >> 32) + (u64Mid >> 32);
	u64Lower = (u64Mid << 32) | (u64Ll & 0xffffffffull);

	*pLo = u64Lower;
	*pHi = u64Upper;
}

/*
 * b1732_add_u64_cout — a + b; store sum; return carry 0/1.
 */
static uint64_t
b1732_add_u64_cout(uint64_t u64A, uint64_t u64B, uint64_t *pSum)
{
	uint64_t u64S;

	u64S = u64A + u64B;
	*pSum = u64S;
	return (u64S < u64A) ? 1ull : 0ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u128_mul_u64_hint — (*lo,*hi) *= m; optional top 64 of 192-bit product.
 *
 * lo, hi: in/out limbs (required)
 * m:      multiplier
 * carry_out: optional high 64 of 192-bit product
 *
 * Returns 0 on success, -1 if lo or hi is NULL.
 */
int
gj_u128_mul_u64_hint(uint64_t *pLo, uint64_t *pHi, uint64_t u64M,
    uint64_t *pCarryOut)
{
	uint64_t u64LoIn;
	uint64_t u64HiIn;
	uint64_t u64P0;
	uint64_t u64P1;
	uint64_t u64P2;
	uint64_t u64P3;
	uint64_t u64Mid;
	uint64_t u64C;
	uint64_t u64Top;

	if (pLo == NULL || pHi == NULL) {
		return -1;
	}

	u64LoIn = *pLo;
	u64HiIn = *pHi;

	/* lo*m → (p0,p1); hi*m → (p2,p3); assemble 192-bit. */
	b1732_umul64x64(u64LoIn, u64M, &u64P0, &u64P1);
	b1732_umul64x64(u64HiIn, u64M, &u64P2, &u64P3);

	u64C = b1732_add_u64_cout(u64P1, u64P2, &u64Mid);
	u64Top = u64P3 + u64C;

	*pLo = u64P0;
	*pHi = u64Mid;
	if (pCarryOut != NULL) {
		*pCarryOut = u64Top;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u128_mul_u64_hint(uint64_t *pLo, uint64_t *pHi, uint64_t u64M,
    uint64_t *pCarryOut)
    __attribute__((alias("gj_u128_mul_u64_hint")));
