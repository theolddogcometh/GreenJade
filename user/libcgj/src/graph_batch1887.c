/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1887: ENOENT errno-number hint.
 *
 * Surface (unique symbols):
 *   int gj_errno_is_noent_hint(int err);
 *     — Return 1 if err is the Linux x86_64 "No such file or directory"
 *       code (ENOENT), else 0.
 *   int __gj_errno_is_noent_hint  (alias)
 *   __libcgj_batch1887_marker = "libcgj-batch1887"
 *
 * Linux x86_64 numeric values used (local constants only; no errno.h):
 *   ENOENT = 2  — No such file or directory
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

const char __libcgj_batch1887_marker[] = "libcgj-batch1887";

/* Linux x86_64: No such file or directory. */
#define B1887_ENOENT  2

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if err equals ENOENT (2). */
static int
b1887_is_noent(int nErr)
{
	return (nErr == B1887_ENOENT) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_is_noent_hint — 1 if err is ENOENT (2).
 *
 * err: positive errno-style code (or any int)
 * Returns 1 for 2, else 0.
 */
int
gj_errno_is_noent_hint(int nErr)
{
	(void)NULL;
	return b1887_is_noent(nErr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_errno_is_noent_hint(int nErr)
    __attribute__((alias("gj_errno_is_noent_hint")));
