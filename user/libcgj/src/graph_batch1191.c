/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1191: error code success predicate (0 is OK).
 *
 * Surface (unique symbols):
 *   int gj_error_code_ok(int code);
 *     — Return 1 if code == 0 (success); else 0.
 *   int __gj_error_code_ok  (alias)
 *   __libcgj_batch1191_marker = "libcgj-batch1191"
 *
 * Generic success predicate; distinct from SMTP/status helpers — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1191_marker[] = "libcgj-batch1191";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_error_code_ok — 1 if code is zero (OK).
 */
int
gj_error_code_ok(int nCode)
{
	return (nCode == 0) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_error_code_ok(int nCode)
    __attribute__((alias("gj_error_code_ok")));
