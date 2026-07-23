/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1453: 2x2 identity matrix (int32 row-major).
 *
 * Surface (unique symbols):
 *   void gj_mat2_i32_identity(int32_t m[4]);
 *     — Write the 2x2 identity into m as 4 row-major int32 values.
 *       NULL m → no-op.
 *   void __gj_mat2_i32_identity  (alias)
 *   __libcgj_batch1453_marker = "libcgj-batch1453"
 *
 * Distinct from gj_mat3_identity_i32 (batch951). Unique surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1453_marker[] = "libcgj-batch1453";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat2_i32_identity — store I_2 into m[4] (row-major).
 *
 * Layout: m[0]=1 m[1]=0
 *         m[2]=0 m[3]=1
 */
void
gj_mat2_i32_identity(int32_t *pM)
{
	if (pM == NULL) {
		return;
	}
	pM[0] = 1;
	pM[1] = 0;
	pM[2] = 0;
	pM[3] = 1;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_mat2_i32_identity(int32_t *pM)
    __attribute__((alias("gj_mat2_i32_identity")));
