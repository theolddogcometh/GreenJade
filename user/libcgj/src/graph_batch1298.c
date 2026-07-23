/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1298: AVL rotation-needed predicate.
 *
 * Surface (unique symbols):
 *   int gj_avl_need_rotate(int bf);
 *     — Return 1 if |bf| > 1 (AVL node needs a rotation), else 0.
 *       bf is a balance factor (typically height_left - height_right).
 *   int __gj_avl_need_rotate  (alias)
 *   __libcgj_batch1298_marker = "libcgj-batch1298"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1298_marker[] = "libcgj-batch1298";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_avl_need_rotate — 1 if absolute balance factor exceeds 1.
 */
int
gj_avl_need_rotate(int nBf)
{
	if (nBf > 1 || nBf < -1) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_avl_need_rotate(int nBf)
    __attribute__((alias("gj_avl_need_rotate")));
