/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch327: single-thread once flag.
 *
 * Surface (unique symbols):
 *   int gj_once(int *flag, void (*fn)(void));
 *     — Non-atomic single-thread once. *flag is the done marker
 *       (0 = not yet, non-zero = already). On the first call with
 *       *flag == 0, invokes fn() then sets *flag = 1 and returns 0.
 *       On a later call with *flag != 0, does not call fn and returns 1.
 *       Returns -1 if flag or fn is NULL (no write, no call).
 *   __gj_once  (alias)
 *   __libcgj_batch327_marker = "libcgj-batch327"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * atomics. Not safe for concurrent callers (use pthread_once / call_once
 * for multi-thread). No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch327_marker[] = "libcgj-batch327";

/*
 * gj_once — run fn exactly once per flag (single-thread).
 *
 * Returns:
 *   0  first call: ran fn, *flag set to 1
 *   1  already done: *flag was non-zero, fn not called
 *  -1  NULL flag or fn
 */
int
gj_once(int *flag, void (*fn)(void))
{
	if (flag == NULL || fn == NULL) {
		return -1;
	}
	if (*flag != 0) {
		return 1;
	}
	fn();
	*flag = 1;
	return 0;
}

int __gj_once(int *flag, void (*fn)(void))
    __attribute__((alias("gj_once")));
