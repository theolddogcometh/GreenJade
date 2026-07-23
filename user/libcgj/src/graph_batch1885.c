/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1885: EINTR errno-number hint.
 *
 * Surface (unique symbols):
 *   int gj_errno_is_intr_hint(int err);
 *     — Return 1 if err is the Linux x86_64 interrupted-system-call
 *       code (EINTR), else 0.
 *   int __gj_errno_is_intr_hint  (alias)
 *   __libcgj_batch1885_marker = "libcgj-batch1885"
 *
 * Linux x86_64 numeric values used (local constants only; no errno.h):
 *   EINTR = 4  — Interrupted system call
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

const char __libcgj_batch1885_marker[] = "libcgj-batch1885";

/* Linux x86_64: Interrupted system call. */
#define B1885_EINTR  4

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if err equals EINTR (4). */
static int
b1885_is_intr(int nErr)
{
	return (nErr == B1885_EINTR) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_is_intr_hint — 1 if err is EINTR (4).
 *
 * err: positive errno-style code (or any int)
 * Returns 1 for 4, else 0.
 */
int
gj_errno_is_intr_hint(int nErr)
{
	(void)NULL;
	return b1885_is_intr(nErr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_errno_is_intr_hint(int nErr)
    __attribute__((alias("gj_errno_is_intr_hint")));
