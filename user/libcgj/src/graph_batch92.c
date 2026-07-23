/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch92: minimal freestanding libm surface for
 * desktop graphics / geometry (sqrt, fabs, floor, ceil).
 * Clean-room pure C + SSE2 sqrtsd/sqrtss. No third-party libm copied.
 *
 * Surface:
 *   double sqrt(double x);   -- hardware sqrtsd
 *   float  sqrtf(float x);   -- hardware sqrtss
 *   double fabs(double x);
 *   float  fabsf(float x);
 *   double floor(double x);
 *   double ceil(double x);
 *   __libcgj_batch92_marker = "libcgj-batch92"
 *
 * Notes:
 *   - Userspace builds already pass -msse2; soft float via SSE is OK.
 *   - floor/ceil use IEEE bit paths (no x87, no libm, careful vs cast).
 *   - No multi-def: grep showed no existing sqrt/fabs/floor/ceil bodies.
 */

#include <stdint.h>

const char __libcgj_batch92_marker[] = "libcgj-batch92";

/* ---- helpers: IEEE bit views ------------------------------------------- */

typedef union {
	double d;
	uint64_t u64;
} b92_dbits;

typedef union {
	float flt;
	uint32_t u32;
} b92_fbits;

/* ---- sqrt / sqrtf: SSE2 hardware --------------------------------------- */

double
sqrt(double dX)
{
	double dY;

	/*
	 * sqrtsd: IEEE sqrt on scalar double in XMM.
	 * Negative → quiet NaN (hardware); +Inf → +Inf; ±0 preserved.
	 */
	__asm__ volatile("sqrtsd %1, %0"
			 : "=x"(dY)
			 : "x"(dX));
	return dY;
}

float
sqrtf(float fltX)
{
	float fltY;

	__asm__ volatile("sqrtss %1, %0"
			 : "=x"(fltY)
			 : "x"(fltX));
	return fltY;
}

/* ---- fabs / fabsf: clear sign bit -------------------------------------- */

double
fabs(double dX)
{
	b92_dbits ub;

	ub.d = dX;
	ub.u64 &= 0x7fffffffffffffffULL;
	return ub.d;
}

float
fabsf(float fltX)
{
	b92_fbits ub;

	ub.flt = fltX;
	ub.u32 &= 0x7fffffffU;
	return ub.flt;
}

/* ---- floor: toward -inf via exponent / mantissa mask ------------------- */

/*
 * Freestanding floor without casting through long long (which fails for
 * |x| >= 2^63 and mishandles NaN/Inf). Mask fractional bits; for negative
 * non-integers bump toward -inf before clearing the fraction field.
 */
double
floor(double dX)
{
	b92_dbits ub;
	uint64_t uBits;
	int nExp;
	uint64_t uMask;

	ub.d = dX;
	uBits = ub.u64;
	nExp = (int)((uBits >> 52) & 0x7ffULL) - 1023;

	/* NaN / Inf / |x| >= 2^52: already integral in double format. */
	if (nExp >= 52) {
		return dX;
	}

	/* |x| < 1 */
	if (nExp < 0) {
		if ((uBits >> 63) != 0ULL) {
			/* negative: -0 stays; other (-1,0) → -1 */
			if ((uBits << 1) != 0ULL) {
				return -1.0;
			}
			return dX;
		}
		/* positive sub-unit → +0 (preserve +0) */
		ub.u64 = 0ULL;
		return ub.d;
	}

	uMask = (1ULL << (52 - nExp)) - 1ULL;
	if ((uBits & uMask) == 0ULL) {
		return dX; /* already integer */
	}

	if ((uBits >> 63) != 0ULL) {
		/* negative non-integer: move toward -inf before strip */
		uBits += uMask;
	}
	uBits &= ~uMask;
	ub.u64 = uBits;
	return ub.d;
}

/* ---- ceil: toward +inf (dual of floor) --------------------------------- */

double
ceil(double dX)
{
	b92_dbits ub;
	uint64_t uBits;
	int nExp;
	uint64_t uMask;

	ub.d = dX;
	uBits = ub.u64;
	nExp = (int)((uBits >> 52) & 0x7ffULL) - 1023;

	if (nExp >= 52) {
		return dX;
	}

	if (nExp < 0) {
		if ((uBits >> 63) != 0ULL) {
			/* (-1, 0]: ceil → -0 */
			ub.u64 = 0x8000000000000000ULL;
			return ub.d;
		}
		/* +0 stays; (0, 1) → +1 */
		if ((uBits << 1) != 0ULL) {
			return 1.0;
		}
		return dX;
	}

	uMask = (1ULL << (52 - nExp)) - 1ULL;
	if ((uBits & uMask) == 0ULL) {
		return dX;
	}

	if ((uBits >> 63) == 0ULL) {
		/* positive non-integer: bump toward +inf before strip */
		uBits += uMask;
	}
	uBits &= ~uMask;
	ub.u64 = uBits;
	return ub.d;
}
