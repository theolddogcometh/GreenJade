/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8535: complex conjugate imag-part stub (_u).
 *
 * Surface (unique symbols):
 *   int32_t gj_cconj_im_u_8535(int32_t im);
 *     - Imag part of conjugate: conj(re + i*im) has imag part -im.
 *       INT32_MIN is returned unchanged (two's-complement magnitude;
 *       no signed negation of INT32_MIN). Soft cconj imag-half surface.
 *   int32_t __gj_cconj_im_u_8535  (alias)
 *   __libcgj_batch8535_marker = "libcgj-batch8535"
 *
 * Exclusive continuum CREATE-ONLY (8531-8540: complex/polar id stubs —
 * integer magnitude-ish only). Unique gj_cconj_im_u_8535 surface only;
 * no multi-def. Pair with gj_cconj_re_u_8534. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8535_marker[] = "libcgj-batch8535";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8535_cconj_im — negate imag for conjugate; INT32_MIN stays INT32_MIN.
 */
static int32_t
b8535_cconj_im(int32_t i32Im)
{
	if (i32Im == INT32_MIN) {
		return INT32_MIN;
	}
	return (int32_t)(-i32Im);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cconj_im_u_8535 - imag part of complex conjugate (integer).
 *
 * im: imaginary component; returns -im (INT32_MIN fixed point).
 *
 * No parent wires.
 */
int32_t
gj_cconj_im_u_8535(int32_t i32Im)
{
	(void)NULL;
	return b8535_cconj_im(i32Im);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_cconj_im_u_8535(int32_t i32Im)
    __attribute__((alias("gj_cconj_im_u_8535")));
