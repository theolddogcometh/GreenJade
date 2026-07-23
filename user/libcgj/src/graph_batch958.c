/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch958: quaternion identity int32 (wxyz, w=1).
 *
 * Surface (unique symbols):
 *   void gj_quat_identity_i32(int32_t q[4]);
 *     — Store identity quaternion as wxyz: q[0]=1, q[1]=0, q[2]=0, q[3]=0.
 *       NULL q → no-op.
 *   void __gj_quat_identity_i32  (alias)
 *   __libcgj_batch958_marker = "libcgj-batch958"

 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch958_marker[] = "libcgj-batch958";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_quat_identity_i32 — identity quaternion in wxyz order (int32).
 *
 * w = 1, x = 0, y = 0, z = 0.
 */
void
gj_quat_identity_i32(int32_t *pQ)
{
	if (pQ == NULL) {
		return;
	}
	pQ[0] = 1;
	pQ[1] = 0;
	pQ[2] = 0;
	pQ[3] = 0;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_quat_identity_i32(int32_t *pQ)
    __attribute__((alias("gj_quat_identity_i32")));
