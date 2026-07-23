/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch983: branch-likely identity (function, not macro).
 *
 * Surface (unique symbols):
 *   int gj_likely(int x);
 *     — Return x unchanged. Function form of a "likely true" branch hint
 *       placeholder for freestanding code that cannot use __builtin_expect
 *       macros at the call site policy layer.
 *   int __gj_likely  (alias)
 *   __libcgj_batch983_marker = "libcgj-batch983"
 *
 * Not a preprocessor macro — unique gj_likely function only.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch983_marker[] = "libcgj-batch983";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_likely — identity; documents that x is expected to be nonzero/true.
 *
 * Returns x as-is. Callers may still wrap with __builtin_expect at their
 * own site; this surface stays a plain function for ABI stability.
 */
int
gj_likely(int nX)
{
	return nX;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_likely(int nX)
    __attribute__((alias("gj_likely")));
