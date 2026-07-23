/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1884: EAGAIN/EWOULDBLOCK errno-number hint.
 *
 * Surface (unique symbols):
 *   int gj_errno_is_again_hint(int err);
 *     — Return 1 if err is the Linux x86_64 resource-temporarily-
 *       unavailable code (EAGAIN / EWOULDBLOCK, same number), else 0.
 *   int __gj_errno_is_again_hint  (alias)
 *   __libcgj_batch1884_marker = "libcgj-batch1884"
 *
 * Linux x86_64 numeric values used (local constants only; no errno.h):
 *   EAGAIN      = 11  — Resource temporarily unavailable
 *   EWOULDBLOCK = 11  — same value as EAGAIN on Linux (alias)
 *
 * Signal/errno exclusive pure-data wave (1881–1889). Distinct from
 * gj_error_code_is_temp (batch1192) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1884_marker[] = "libcgj-batch1884";

/*
 * Linux x86_64: EAGAIN and EWOULDBLOCK share one number.
 * Both macros kept for documentation of the dual name.
 */
#define B1884_EAGAIN       11
#define B1884_EWOULDBLOCK  11

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if err equals EAGAIN/EWOULDBLOCK (11). */
static int
b1884_is_again(int nErr)
{
	/* EWOULDBLOCK == EAGAIN on Linux; single compare covers both names. */
	if (nErr == B1884_EAGAIN || nErr == B1884_EWOULDBLOCK) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_is_again_hint — 1 if err is EAGAIN/EWOULDBLOCK (11).
 *
 * err: positive errno-style code (or any int)
 * Returns 1 for 11, else 0.
 */
int
gj_errno_is_again_hint(int nErr)
{
	(void)NULL;
	return b1884_is_again(nErr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_errno_is_again_hint(int nErr)
    __attribute__((alias("gj_errno_is_again_hint")));
