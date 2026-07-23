/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1193: status success predicate.
 *
 * Surface (unique symbols):
 *   int gj_status_is_success(int status);
 *     — Return 1 if status == 0; else 0.
 *   int __gj_status_is_success  (alias)
 *   __libcgj_batch1193_marker = "libcgj-batch1193"
 *
 * Distinct from gj_smtp_is_success / gj_error_code_ok — unique name; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1193_marker[] = "libcgj-batch1193";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_status_is_success — 1 if status is success (zero).
 */
int
gj_status_is_success(int nStatus)
{
	return (nStatus == 0) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_status_is_success(int nStatus)
    __attribute__((alias("gj_status_is_success")));
