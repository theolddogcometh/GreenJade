/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8534: complex conjugate real-part stub (_u).
 *
 * Surface (unique symbols):
 *   int32_t gj_cconj_re_u_8534(int32_t re);
 *     - Real part of conjugate: conj(re + i*im) has real part re
 *       (unchanged). Soft cconj real-half integer surface.
 *   int32_t __gj_cconj_re_u_8534  (alias)
 *   __libcgj_batch8534_marker = "libcgj-batch8534"
 *
 * Exclusive continuum CREATE-ONLY (8531-8540: complex/polar id stubs —
 * integer magnitude-ish only). Unique gj_cconj_re_u_8534 surface only;
 * no multi-def. Pair with gj_cconj_im_u_8535. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8534_marker[] = "libcgj-batch8534";

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b8534_cconj_re(int32_t i32Re)
{
	/* conj does not flip the real part. */
	return i32Re;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cconj_re_u_8534 - real part of complex conjugate (integer).
 *
 * re: real component; returned unchanged.
 *
 * No parent wires.
 */
int32_t
gj_cconj_re_u_8534(int32_t i32Re)
{
	(void)NULL;
	return b8534_cconj_re(i32Re);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_cconj_re_u_8534(int32_t i32Re)
    __attribute__((alias("gj_cconj_re_u_8534")));
