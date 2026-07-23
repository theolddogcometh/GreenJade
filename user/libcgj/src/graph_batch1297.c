/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1297: BST / AVL balance-factor helper.
 *
 * Surface (unique symbols):
 *   int gj_bst_balance_factor(int h_left, int h_right);
 *     — Balance factor = h_left - h_right (AVL convention). Heights are
 *       treated as signed subtree heights; caller supplies them.
 *   int __gj_bst_balance_factor  (alias)
 *   __libcgj_batch1297_marker = "libcgj-batch1297"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1297_marker[] = "libcgj-batch1297";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bst_balance_factor — h_left - h_right.
 */
int
gj_bst_balance_factor(int nHLeft, int nHRight)
{
	return nHLeft - nHRight;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bst_balance_factor(int nHLeft, int nHRight)
    __attribute__((alias("gj_bst_balance_factor")));
