/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch951: 3x3 identity matrix (int32 row-major).
 *
 * Surface (unique symbols):
 *   void gj_mat3_identity_i32(int32_t m[9]);
 *     — Write the 3x3 identity into m as 9 row-major int32 values.
 *       NULL m → no-op.
 *   void __gj_mat3_identity_i32  (alias)
 *   __libcgj_batch951_marker = "libcgj-batch951"

 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch951_marker[] = "libcgj-batch951";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat3_identity_i32 — store I_3 into m[9] (row-major).
 *
 * Layout: m[0]=1 m[1]=0 m[2]=0
 *         m[3]=0 m[4]=1 m[5]=0
 *         m[6]=0 m[7]=0 m[8]=1
 */
void
gj_mat3_identity_i32(int32_t *pM)
{
	if (pM == NULL) {
		return;
	}
	pM[0] = 1;
	pM[1] = 0;
	pM[2] = 0;
	pM[3] = 0;
	pM[4] = 1;
	pM[5] = 0;
	pM[6] = 0;
	pM[7] = 0;
	pM[8] = 1;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_mat3_identity_i32(int32_t *pM)
    __attribute__((alias("gj_mat3_identity_i32")));
