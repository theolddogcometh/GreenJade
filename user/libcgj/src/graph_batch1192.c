/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1192: temporary / retryable error code classifier.
 *
 * Surface (unique symbols):
 *   int gj_error_code_is_temp(int code);
 *     — Return 1 if code is a temporary/retryable error, else 0.
 *       Recognizes GreenJade-style negatives:
 *         GJ_ERR_AGAIN(-5), GJ_ERR_BUSY(-7), GJ_ERR_TIMEOUT(-12)
 *       and positive POSIX-style values commonly used freestanding:
 *         EINTR(4), EAGAIN(11), EBUSY(16), ETIMEDOUT(110).
 *       Success (0) and other codes → 0.
 *   int __gj_error_code_is_temp  (alias)
 *   __libcgj_batch1192_marker = "libcgj-batch1192"
 *
 * Local classification constants only (no errno.h). Unique name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1192_marker[] = "libcgj-batch1192";

/* GreenJade error.h style (negatives). */
#define B1192_GJ_AGAIN    (-5)
#define B1192_GJ_BUSY     (-7)
#define B1192_GJ_TIMEOUT  (-12)

/* Common positive errno-style values (no libc include). */
#define B1192_EINTR       4
#define B1192_EAGAIN      11
#define B1192_EBUSY       16
#define B1192_ETIMEDOUT   110

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_error_code_is_temp — 1 if code should be retried.
 */
int
gj_error_code_is_temp(int nCode)
{
	if (nCode == B1192_GJ_AGAIN || nCode == B1192_GJ_BUSY ||
	    nCode == B1192_GJ_TIMEOUT) {
		return 1;
	}
	if (nCode == B1192_EINTR || nCode == B1192_EAGAIN ||
	    nCode == B1192_EBUSY || nCode == B1192_ETIMEDOUT) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_error_code_is_temp(int nCode)
    __attribute__((alias("gj_error_code_is_temp")));
