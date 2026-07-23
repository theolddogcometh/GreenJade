/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8532: complex real-part identity stub.
 *
 * Surface (unique symbols):
 *   int32_t gj_creal_id_8532(int32_t re);
 *     - Identity on the real part of a complex integer: returns re
 *       unchanged. Soft polar/complex catalog surface (creal-shaped).
 *   int32_t __gj_creal_id_8532  (alias)
 *   __libcgj_batch8532_marker = "libcgj-batch8532"
 *
 * Exclusive continuum CREATE-ONLY (8531-8540: complex/polar id stubs —
 * integer magnitude-ish only). Unique gj_creal_id_8532 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8532_marker[] = "libcgj-batch8532";

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b8532_creal_id(int32_t i32Re)
{
	return i32Re;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_creal_id_8532 - real-part identity (creal-shaped integer stub).
 *
 * re: real component of a complex integer
 *
 * Returns re unchanged. Pair with gj_cimag_id_8533 for the imag half.
 * No parent wires.
 */
int32_t
gj_creal_id_8532(int32_t i32Re)
{
	(void)NULL;
	return b8532_creal_id(i32Re);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_creal_id_8532(int32_t i32Re)
    __attribute__((alias("gj_creal_id_8532")));
