/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1354: elegant (exponential) unpairing.
 *
 * Surface (unique symbols):
 *   void gj_elegant_unpair(uint64_t z, uint32_t *x, uint32_t *y);
 *     — Invert π(x,y) = 2^x * (2*y + 1) - 1.
 *       x = ctz(z+1), y = (((z+1) >> x) - 1) / 2.
 *       NULL out-pointers ignored.
 *   void __gj_elegant_unpair  (alias)
 *   __libcgj_batch1354_marker = "libcgj-batch1354"
 *
 * Distinct from Cantor/Szudzik unpair. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler ctz builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1354_marker[] = "libcgj-batch1354";

/* ---- freestanding helpers ---------------------------------------------- */

/* Count trailing zeros in a non-zero 64-bit word (binary peel). */
static unsigned
b1354_ctz64(uint64_t u64V)
{
	unsigned uN;

	uN = 0u;
	if ((u64V & 0xffffffffull) == 0ull) {
		uN += 32u;
		u64V >>= 32;
	}
	if ((u64V & 0xffffull) == 0ull) {
		uN += 16u;
		u64V >>= 16;
	}
	if ((u64V & 0xffull) == 0ull) {
		uN += 8u;
		u64V >>= 8;
	}
	if ((u64V & 0xfull) == 0ull) {
		uN += 4u;
		u64V >>= 4;
	}
	if ((u64V & 0x3ull) == 0ull) {
		uN += 2u;
		u64V >>= 2;
	}
	if ((u64V & 0x1ull) == 0ull) {
		uN += 1u;
	}
	return uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elegant_unpair — invert elegant exponential pairing.
 *
 * z+1 = 2^x * (2y+1); x is the trailing-zero count of z+1.
 * UINT64_MAX as input (encode overflow sentinel) yields (0, UINT32_MAX)
 * for a recoverable defensive decode path.
 */
void
gj_elegant_unpair(uint64_t u64Z, uint32_t *pX, uint32_t *pY)
{
	uint64_t u64W;
	unsigned uX;
	uint64_t u64Odd;
	uint64_t u64Y;

	if (u64Z == UINT64_MAX) {
		if (pX != NULL) {
			*pX = 0u;
		}
		if (pY != NULL) {
			*pY = 0xffffffffu;
		}
		return;
	}

	u64W = u64Z + 1ull;
	uX = b1354_ctz64(u64W);
	u64Odd = u64W >> uX;
	u64Y = (u64Odd - 1ull) / 2ull;

	if (pX != NULL) {
		*pX = (uint32_t)uX;
	}
	if (pY != NULL) {
		*pY = (u64Y > 0xffffffffull) ? 0xffffffffu : (uint32_t)u64Y;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_elegant_unpair(uint64_t u64Z, uint32_t *pX, uint32_t *pY)
    __attribute__((alias("gj_elegant_unpair")));
