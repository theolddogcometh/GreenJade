/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch241: Z-algorithm (linear-time Z-array).
 *
 * Surface (unique symbols):
 *   int gj_z_array(const char *s, size_t n, int *z);
 *     — fill z[0..n-1] for string s of length n. z[i] is the length of
 *       the longest substring starting at i that matches a prefix of s.
 *       Convention: z[0] = 0 (not n). Returns 0 on success, -1 on bad
 *       args (NULL s/z or n > 512). n == 0 is success with no writes.
 *   __gj_z_array  (alias)
 *   __libcgj_batch241_marker = "libcgj-batch241"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. Stack-only (n <= 512).
 * No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch241_marker[] = "libcgj-batch241";

/* Maximum string length accepted by gj_z_array (stack / API bound). */
#define B241_Z_MAX 512u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b241_z_fill — classic linear Z-box construction.
 *
 * Maintains the rightmost segment [L, R] that matches a prefix of s.
 * For each i > 0, either copy from the corresponding z-box entry or
 * explicitly extend the match past R. z[0] is set to 0 by convention.
 * Caller guarantees s and z non-NULL and n <= B241_Z_MAX.
 */
static void
b241_z_fill(const char *s, size_t n, int *z)
{
	size_t i;
	size_t L;
	size_t R;

	if (n == 0u) {
		return;
	}

	z[0] = 0;
	L = 0u;
	R = 0u;

	for (i = 1u; i < n; i++) {
		if (i > R) {
			/* Outside the Z-box: explicit match from s[0]. */
			L = i;
			R = i;
			while (R < n && s[R - L] == s[R]) {
				R++;
			}
			z[i] = (int)(R - L);
			R--;
		} else {
			size_t k = i - L;
			size_t rem = R - i + 1u;

			if ((size_t)z[k] < rem) {
				/* Fully contained in the Z-box. */
				z[i] = z[k];
			} else {
				/* May extend past R. */
				L = i;
				while (R < n && s[R - L] == s[R]) {
					R++;
				}
				z[i] = (int)(R - L);
				R--;
			}
		}
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_z_array — compute the Z-array for s[0..n-1].
 *
 * s: input string (not necessarily NUL-terminated; length is n).
 * n: length in bytes; must be <= 512.
 * z: output array of at least n ints (caller-owned).
 *
 * Returns 0 on success. Returns -1 if s is NULL, z is NULL, or n > 512.
 * On success with n > 0: z[0] = 0 and z[i] is the Z-value at i for i>0.
 * On success with n == 0: no writes to z.
 */
int
gj_z_array(const char *s, size_t n, int *z)
{
	if (s == NULL || z == NULL || n > (size_t)B241_Z_MAX) {
		return -1;
	}
	b241_z_fill(s, n, z);
	return 0;
}

int __gj_z_array(const char *s, size_t n, int *z)
    __attribute__((alias("gj_z_array")));
