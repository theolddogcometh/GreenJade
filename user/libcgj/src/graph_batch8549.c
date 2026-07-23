/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8549: IEEE-754 binary32 fpclassify-style id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fp_classify_id_8549(uint32_t bits);
 *     - Classify an IEEE-754 binary32 bit pattern into a small integer
 *       id (libc-compatible fpclassify order, pure integer bits):
 *         0 = NaN, 1 = infinite, 2 = zero, 3 = subnormal, 4 = normal.
 *   uint32_t __gj_fp_classify_id_8549  (alias)
 *   __libcgj_batch8549_marker = "libcgj-batch8549"
 *
 * Exclusive continuum CREATE-ONLY (8541-8550: math classify integer
 * stubs on IEEE binary32 bits as uint32). Unique
 * gj_fp_classify_id_8549 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8549_marker[] = "libcgj-batch8549";

/* binary32 masks. */
#define B8549_F32_ABS       0x7fffffffu
#define B8549_F32_EXP       0x7f800000u
#define B8549_F32_EXP_SHIFT 23u

/* fpclassify-compatible class ids (integer-only, no math.h). */
#define B8549_ID_NAN       0u
#define B8549_ID_INFINITE  1u
#define B8549_ID_ZERO      2u
#define B8549_ID_SUBNORMAL 3u
#define B8549_ID_NORMAL    4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8549_classify(uint32_t uBits)
{
	uint32_t uAbs;
	uint32_t uExp;

	uAbs = uBits & B8549_F32_ABS;
	uExp = uAbs >> B8549_F32_EXP_SHIFT;

	if (uExp == 255u) {
		/* Max exp: nonzero frac => NaN, else Inf. */
		return (uAbs > B8549_F32_EXP) ? B8549_ID_NAN
					      : B8549_ID_INFINITE;
	}
	if (uExp == 0u) {
		/* Min exp: zero frac => ±0, else subnormal. */
		return (uAbs == 0u) ? B8549_ID_ZERO : B8549_ID_SUBNORMAL;
	}
	return B8549_ID_NORMAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fp_classify_id_8549 - classify binary32 bits into an integer class id.
 *
 * bits: IEEE-754 binary32 encoding as uint32_t
 * Returns 0=NaN, 1=Inf, 2=zero, 3=subnormal, 4=normal.
 */
uint32_t
gj_fp_classify_id_8549(uint32_t bits)
{
	(void)NULL;
	return b8549_classify(bits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fp_classify_id_8549(uint32_t bits)
    __attribute__((alias("gj_fp_classify_id_8549")));
