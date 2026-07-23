/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch984: branch-unlikely identity (function, not macro).
 *
 * Surface (unique symbols):
 *   int gj_unlikely(int x);
 *     — Return x unchanged. Function form of a "likely false" branch hint
 *       placeholder (pair of gj_likely in batch983).
 *   int __gj_unlikely  (alias)
 *   __libcgj_batch984_marker = "libcgj-batch984"
 *
 * Does NOT redefine gj_likely (batch983). Not a preprocessor macro.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch984_marker[] = "libcgj-batch984";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unlikely — identity; documents that x is expected to be zero/false.
 *
 * Returns x as-is.
 */
int
gj_unlikely(int nX)
{
	return nX;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_unlikely(int nX)
    __attribute__((alias("gj_unlikely")));
