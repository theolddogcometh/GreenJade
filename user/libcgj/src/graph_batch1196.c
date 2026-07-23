/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1196: retry loop stop predicate.
 *
 * Surface (unique symbols):
 *   int gj_retry_should_stop(unsigned attempt, unsigned max_attempts);
 *     — Return 1 if attempt >= max_attempts (caller should stop).
 *       max_attempts==0 → always stop (1). attempt is 0-based count
 *       of tries already performed, or 1-based; either works if the
 *       same convention is used for both args.
 *   int __gj_retry_should_stop  (alias)
 *   __libcgj_batch1196_marker = "libcgj-batch1196"
 *
 * Pairs with gj_retry_backoff_us (batch1195); unique name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1196_marker[] = "libcgj-batch1196";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_retry_should_stop — 1 if no more retries should be attempted.
 */
int
gj_retry_should_stop(unsigned uAttempt, unsigned uMaxAttempts)
{
	if (uMaxAttempts == 0u) {
		return 1;
	}
	return (uAttempt >= uMaxAttempts) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_retry_should_stop(unsigned uAttempt, unsigned uMaxAttempts)
    __attribute__((alias("gj_retry_should_stop")));
