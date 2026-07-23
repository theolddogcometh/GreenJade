/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1888: EEXIST errno-number hint.
 *
 * Surface (unique symbols):
 *   int gj_errno_is_exist_hint(int err);
 *     — Return 1 if err is the Linux x86_64 "File exists" code (EEXIST),
 *       else 0.
 *   int __gj_errno_is_exist_hint  (alias)
 *   __libcgj_batch1888_marker = "libcgj-batch1888"
 *
 * Linux x86_64 numeric values used (local constants only; no errno.h):
 *   EEXIST = 17  — File exists
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

const char __libcgj_batch1888_marker[] = "libcgj-batch1888";

/* Linux x86_64: File exists. */
#define B1888_EEXIST  17

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if err equals EEXIST (17). */
static int
b1888_is_exist(int nErr)
{
	return (nErr == B1888_EEXIST) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_is_exist_hint — 1 if err is EEXIST (17).
 *
 * err: positive errno-style code (or any int)
 * Returns 1 for 17, else 0.
 */
int
gj_errno_is_exist_hint(int nErr)
{
	(void)NULL;
	return b1888_is_exist(nErr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_errno_is_exist_hint(int nErr)
    __attribute__((alias("gj_errno_is_exist_hint")));
