/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3314: extended Euclidean GCD on uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_egcd_u(uint32_t a, uint32_t b, int32_t *x, int32_t *y);
 *     — extended Euclidean: returns g = gcd(a, b) and writes (if non-NULL)
 *       integers x, y such that (int32_t)a * x + (int32_t)b * y == (int32_t)g
 *       when a and b fit in int32_t (both < 2^31). For full-range uint32
 *       inputs the returned g is still exact; x and y outputs are
 *       best-effort and may not satisfy the identity if intermediate
 *       coeffs overflow int32_t (in that case written as 0 if non-NULL).
 *   uint32_t __gj_u32_egcd_u  (alias)
 *   __libcgj_batch3314_marker = "libcgj-batch3314"
 *
 * CREATE-ONLY exclusive continuum wave (3311–3320). Unique gj_u32_egcd_u
 * surface only; no multi-def. Distinct from gj_egcd (batch229). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3314_marker[] = "libcgj-batch3314";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft signed multiply of q * s into *pOut.
 * Returns 1 on success, 0 if product would overflow int32_t.
 */
static int
b3314_smul_ok(int32_t i32Q, int32_t i32S, int32_t *pOut)
{
	if (i32Q == 0 || i32S == 0) {
		*pOut = 0;
		return 1;
	}
	/* INT32_MIN / -1 is undefined; reject that pair. */
	if (i32Q == -1 && i32S == INT32_MIN) {
		return 0;
	}
	if (i32S == -1 && i32Q == INT32_MIN) {
		return 0;
	}
	if (i32Q > 0 && i32S > 0) {
		if (i32Q > INT32_MAX / i32S) {
			return 0;
		}
	} else if (i32Q < 0 && i32S < 0) {
		if (i32Q < INT32_MAX / i32S) {
			return 0;
		}
	} else if (i32S > 0) { /* q < 0 */
		if (i32Q < INT32_MIN / i32S) {
			return 0;
		}
	} else { /* q > 0, s < 0 */
		if (i32S < INT32_MIN / i32Q) {
			return 0;
		}
	}
	*pOut = i32Q * i32S;
	return 1;
}

/*
 * Soft signed subtract a - b into *pOut.
 * Returns 1 on success, 0 on overflow.
 */
static int
b3314_ssub_ok(int32_t i32A, int32_t i32B, int32_t *pOut)
{
	if (i32B > 0) {
		if (i32A < INT32_MIN + i32B) {
			return 0;
		}
	} else if (i32B < 0) {
		if (i32A > INT32_MAX + i32B) {
			return 0;
		}
	}
	*pOut = i32A - i32B;
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_egcd_u — extended Euclidean algorithm on uint32_t.
 *
 * Returns g = gcd(a, b). When x and/or y are non-NULL and the iterative
 * coefficients remain representable as int32_t, writes x, y satisfying
 *   a*x + b*y = g
 * (with a, b interpreted non-negative). If a coefficient would overflow
 * int32_t, both outputs are written as 0 and g is still returned.
 *
 * For a = b = 0: g = 0, x = 0, y = 0.
 * For b = 0, a > 0: g = a, x = 1, y = 0  (when a fits in int32).
 * For a = 0, b > 0: g = b, x = 0, y = 1  (when b fits in int32).
 */
uint32_t
gj_u32_egcd_u(uint32_t u32A, uint32_t u32B, int32_t *pX, int32_t *pY)
{
	uint32_t u32OldR;
	uint32_t u32R;
	int32_t i32OldS;
	int32_t i32S;
	int32_t i32OldT;
	int32_t i32T;
	int iOverflow;

	(void)NULL;

	if (u32A == 0u && u32B == 0u) {
		if (pX != NULL) {
			*pX = 0;
		}
		if (pY != NULL) {
			*pY = 0;
		}
		return 0u;
	}

	u32OldR = u32A;
	u32R = u32B;
	i32OldS = 1;
	i32S = 0;
	i32OldT = 0;
	i32T = 1;
	iOverflow = 0;

	while (u32R != 0u) {
		uint32_t u32Q = u32OldR / u32R;
		uint32_t u32NextR = u32OldR - u32Q * u32R;
		int32_t i32NextS = 0;
		int32_t i32NextT = 0;

		if (u32Q > (uint32_t)INT32_MAX) {
			iOverflow = 1;
			u32OldR = u32R;
			u32R = u32NextR;
			i32OldS = 0;
			i32S = 0;
			i32OldT = 0;
			i32T = 0;
			continue;
		}

		if (iOverflow == 0) {
			int32_t i32Q = (int32_t)u32Q;
			int32_t i32Qs = 0;
			int32_t i32Qt = 0;
			int iOk = 1;

			if (b3314_smul_ok(i32Q, i32S, &i32Qs) == 0) {
				iOk = 0;
			}
			if (iOk != 0 && b3314_ssub_ok(i32OldS, i32Qs, &i32NextS) == 0) {
				iOk = 0;
			}
			if (iOk != 0 && b3314_smul_ok(i32Q, i32T, &i32Qt) == 0) {
				iOk = 0;
			}
			if (iOk != 0 && b3314_ssub_ok(i32OldT, i32Qt, &i32NextT) == 0) {
				iOk = 0;
			}
			if (iOk == 0) {
				iOverflow = 1;
				i32NextS = 0;
				i32NextT = 0;
				i32OldS = 0;
				i32S = 0;
				i32OldT = 0;
				i32T = 0;
			} else {
				i32OldS = i32S;
				i32S = i32NextS;
				i32OldT = i32T;
				i32T = i32NextT;
			}
		}

		u32OldR = u32R;
		u32R = u32NextR;
	}

	/*
	 * When overflow abandoned coeffs, force 0. Otherwise old_s/old_t
	 * are the Bézout pair for old_r = gcd.
	 */
	if (iOverflow != 0) {
		if (pX != NULL) {
			*pX = 0;
		}
		if (pY != NULL) {
			*pY = 0;
		}
	} else {
		if (pX != NULL) {
			*pX = i32OldS;
		}
		if (pY != NULL) {
			*pY = i32OldT;
		}
	}
	return u32OldR;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_egcd_u(uint32_t u32A, uint32_t u32B, int32_t *pX, int32_t *pY)
    __attribute__((alias("gj_u32_egcd_u")));
