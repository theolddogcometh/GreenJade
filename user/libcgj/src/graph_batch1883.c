/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1883: valid signal-number range predicate.
 *
 * Surface (unique symbols):
 *   int gj_signo_ok_range(int signo);
 *     — Return 1 if signo is in the conventional Linux real-time-inclusive
 *       signal range 1..64 inclusive, else 0. (0 is not a valid signal.)
 *   int __gj_signo_ok_range  (alias)
 *   __libcgj_batch1883_marker = "libcgj-batch1883"
 *
 * Linux x86_64 numeric values used (local constants only; no signal.h):
 *   SIG_MIN = 1   — first valid standard signal (SIGHUP)
 *   SIG_MAX = 64  — last real-time signal slot on common Linux configs
 *                   (_NSIG is often 65; valid numbers are 1..64)
 *
 * Signal/errno exclusive pure-data wave (1881–1889). Distinct names —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1883_marker[] = "libcgj-batch1883";

/* Linux x86_64 conventional valid signal number bounds. */
#define B1883_SIG_MIN  1
#define B1883_SIG_MAX  64

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if signo is in [1, 64]. */
static int
b1883_ok_range(int nSigno)
{
	if (nSigno >= B1883_SIG_MIN && nSigno <= B1883_SIG_MAX) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_signo_ok_range — 1 if signo is a conventional valid signal number.
 *
 * signo: candidate signal number
 * Returns 1 for 1..64 inclusive, else 0 (including 0 and negatives).
 */
int
gj_signo_ok_range(int nSigno)
{
	(void)NULL;
	return b1883_ok_range(nSigno);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_signo_ok_range(int nSigno)
    __attribute__((alias("gj_signo_ok_range")));
