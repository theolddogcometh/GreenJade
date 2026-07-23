/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1459: 2-vector scalar multiply int32.
 *
 * Surface (unique symbols):
 *   void gj_vec2_i32_scale(const int32_t v[2], int32_t s, int32_t out[2]);
 *     — out = s * v element-wise. NULL v or out → no-op. out may alias v.
 *   void __gj_vec2_i32_scale  (alias)
 *   __libcgj_batch1459_marker = "libcgj-batch1459"
 *
 * Distinct from gj_mat_scale_i32 (batch442). Unique surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1459_marker[] = "libcgj-batch1459";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vec2_i32_scale — scale a 2-vector by int32 scalar into out.
 *
 * Uses temps so out may overlap v. Element-wise int32 wrap on overflow.
 */
void
gj_vec2_i32_scale(const int32_t *pV, int32_t i32S, int32_t *pOut)
{
	int32_t i32X;
	int32_t i32Y;

	if (pV == NULL || pOut == NULL) {
		return;
	}

	i32X = pV[0] * i32S;
	i32Y = pV[1] * i32S;
	pOut[0] = i32X;
	pOut[1] = i32Y;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_vec2_i32_scale(const int32_t *pV, int32_t i32S, int32_t *pOut)
    __attribute__((alias("gj_vec2_i32_scale")));
