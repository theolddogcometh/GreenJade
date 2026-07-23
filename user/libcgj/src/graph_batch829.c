/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch829: ASCII letter alphabetic index.
 *
 * Surface (unique symbols):
 *   int gj_alpha_val(int c);
 *     — 'A'/'a' → 0 ... 'Z'/'z' → 25; otherwise -1. Case-insensitive.
 *   int __gj_alpha_val  (alias)
 *   __libcgj_batch829_marker = "libcgj-batch829"
 *
 * Companion to gj_digit_val (batch828). Unique name; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch829_marker[] = "libcgj-batch829";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_alpha_val — 0..25 for A-Z/a-z, else -1.
 */
int
gj_alpha_val(int nC)
{
	if (nC >= (int)'A' && nC <= (int)'Z') {
		return nC - (int)'A';
	}
	if (nC >= (int)'a' && nC <= (int)'z') {
		return nC - (int)'a';
	}
	return -1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_alpha_val(int nC)
    __attribute__((alias("gj_alpha_val")));
