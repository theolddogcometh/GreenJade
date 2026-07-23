/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1641: u64 → base-N digit string (2..36).
 *
 * Surface (unique symbols):
 *   size_t gj_u64_to_base_digits(uint64_t v, unsigned base,
 *                                char *out, size_t outcap);
 *     — Write the base-`base` representation of v into out (no leading
 *       zeros except v == 0 → single '0'). Digits: '0'..'9' then 'A'..'Z'.
 *       No trailing NUL. base must be in 2..36; out non-NULL; outcap must
 *       hold the full digit run. Returns length written, or 0 on error.
 *   size_t __gj_u64_to_base_digits  (alias)
 *   __libcgj_batch1641_marker = "libcgj-batch1641"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1641_marker[] = "libcgj-batch1641";

/* u64 in base 2 needs at most 64 digits. */
#define B1641_DIG_MAX 64u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1641_base_ok(unsigned uBase)
{
	return (uBase >= 2u && uBase <= 36u) ? 1 : 0;
}

static char
b1641_digit(unsigned uVal)
{
	if (uVal < 10u) {
		return (char)('0' + uVal);
	}
	return (char)('A' + (uVal - 10u));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_to_base_digits — format v in base 2..36 into out.
 *
 * Digits are written MSB-first with no NUL. On any error (bad base, NULL
 * out, insufficient outcap) returns 0 and leaves out untouched.
 */
size_t
gj_u64_to_base_digits(uint64_t u64V, unsigned uBase, char *pOut,
    size_t cbOut)
{
	char aRev[B1641_DIG_MAX];
	size_t cDig;
	size_t i;
	uint64_t u;

	if (pOut == NULL || !b1641_base_ok(uBase)) {
		return 0u;
	}

	if (u64V == 0ull) {
		if (cbOut < 1u) {
			return 0u;
		}
		pOut[0] = '0';
		return 1u;
	}

	u = u64V;
	cDig = 0u;
	while (u != 0ull) {
		unsigned uRem;

		uRem = (unsigned)(u % (uint64_t)uBase);
		u /= (uint64_t)uBase;
		aRev[cDig++] = b1641_digit(uRem);
	}

	if (cDig > cbOut) {
		return 0u;
	}

	for (i = 0u; i < cDig; i++) {
		pOut[i] = aRev[cDig - 1u - i];
	}
	return cDig;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u64_to_base_digits(uint64_t u64V, unsigned uBase, char *pOut,
    size_t cbOut)
    __attribute__((alias("gj_u64_to_base_digits")));
