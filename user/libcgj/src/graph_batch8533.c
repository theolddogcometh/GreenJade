/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8533: complex imag-part identity stub.
 *
 * Surface (unique symbols):
 *   int32_t gj_cimag_id_8533(int32_t im);
 *     - Identity on the imaginary part of a complex integer: returns im
 *       unchanged. Soft polar/complex catalog surface (cimag-shaped).
 *   int32_t __gj_cimag_id_8533  (alias)
 *   __libcgj_batch8533_marker = "libcgj-batch8533"
 *
 * Exclusive continuum CREATE-ONLY (8531-8540: complex/polar id stubs —
 * integer magnitude-ish only). Unique gj_cimag_id_8533 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8533_marker[] = "libcgj-batch8533";

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b8533_cimag_id(int32_t i32Im)
{
	return i32Im;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cimag_id_8533 - imag-part identity (cimag-shaped integer stub).
 *
 * im: imaginary component of a complex integer
 *
 * Returns im unchanged. Pair with gj_creal_id_8532 for the real half.
 * No parent wires.
 */
int32_t
gj_cimag_id_8533(int32_t i32Im)
{
	(void)NULL;
	return b8533_cimag_id(i32Im);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_cimag_id_8533(int32_t i32Im)
    __attribute__((alias("gj_cimag_id_8533")));
