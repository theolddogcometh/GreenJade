/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1889: EPIPE errno-number hint.
 *
 * Surface (unique symbols):
 *   int gj_errno_is_pipe_hint(int err);
 *     — Return 1 if err is the Linux x86_64 broken-pipe code (EPIPE),
 *       else 0.
 *   int __gj_errno_is_pipe_hint  (alias)
 *   __libcgj_batch1889_marker = "libcgj-batch1889"
 *
 * Linux x86_64 numeric values used (local constants only; no errno.h):
 *   EPIPE = 32  — Broken pipe
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

const char __libcgj_batch1889_marker[] = "libcgj-batch1889";

/* Linux x86_64: Broken pipe. */
#define B1889_EPIPE  32

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if err equals EPIPE (32). */
static int
b1889_is_pipe(int nErr)
{
	return (nErr == B1889_EPIPE) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_is_pipe_hint — 1 if err is EPIPE (32).
 *
 * err: positive errno-style code (or any int)
 * Returns 1 for 32, else 0.
 */
int
gj_errno_is_pipe_hint(int nErr)
{
	(void)NULL;
	return b1889_is_pipe(nErr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_errno_is_pipe_hint(int nErr)
    __attribute__((alias("gj_errno_is_pipe_hint")));
