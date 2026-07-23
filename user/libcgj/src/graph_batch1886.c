/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1886: permission errno-number hint.
 *
 * Surface (unique symbols):
 *   int gj_errno_is_perm_hint(int err);
 *     — Return 1 if err is a Linux x86_64 permission-denied style code
 *       (EACCES or EPERM), else 0.
 *   int __gj_errno_is_perm_hint  (alias)
 *   __libcgj_batch1886_marker = "libcgj-batch1886"
 *
 * Linux x86_64 numeric values used (local constants only; no errno.h):
 *   EPERM  = 1   — Operation not permitted
 *   EACCES = 13  — Permission denied
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

const char __libcgj_batch1886_marker[] = "libcgj-batch1886";

/* Linux x86_64: permission-related errno numbers. */
#define B1886_EPERM   1
#define B1886_EACCES  13

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if err is EPERM or EACCES. */
static int
b1886_is_perm(int nErr)
{
	if (nErr == B1886_EPERM || nErr == B1886_EACCES) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_is_perm_hint — 1 if err is EPERM(1) or EACCES(13).
 *
 * err: positive errno-style code (or any int)
 * Returns 1 for permission-class codes, else 0.
 */
int
gj_errno_is_perm_hint(int nErr)
{
	(void)NULL;
	return b1886_is_perm(nErr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_errno_is_perm_hint(int nErr)
    __attribute__((alias("gj_errno_is_perm_hint")));
