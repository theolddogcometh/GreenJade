/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch957: 3-vector squared length int32.
 *
 * Surface (unique symbols):
 *   int32_t gj_vec3_len2_i32(const int32_t v[3]);
 *     — Return v0*v0 + v1*v1 + v2*v2. NULL v → 0.
 *   int32_t __gj_vec3_len2_i32  (alias)
 *   __libcgj_batch957_marker = "libcgj-batch957"

 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch957_marker[] = "libcgj-batch957";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vec3_len2_i32 — squared Euclidean length of a 3-vector (int32).
 */
int32_t
gj_vec3_len2_i32(const int32_t *pV)
{
	if (pV == NULL) {
		return 0;
	}
	return pV[0] * pV[0] + pV[1] * pV[1] + pV[2] * pV[2];
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_vec3_len2_i32(const int32_t *pV)
    __attribute__((alias("gj_vec3_len2_i32")));
