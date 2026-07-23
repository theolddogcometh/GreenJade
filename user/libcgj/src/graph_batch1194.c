/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1194: status failure predicate.
 *
 * Surface (unique symbols):
 *   int gj_status_is_failure(int status);
 *     — Return 1 if status != 0; else 0.
 *   int __gj_status_is_failure  (alias)
 *   __libcgj_batch1194_marker = "libcgj-batch1194"
 *
 * Complement of gj_status_is_success (batch1193); unique name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1194_marker[] = "libcgj-batch1194";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_status_is_failure — 1 if status is non-zero (failure).
 */
int
gj_status_is_failure(int nStatus)
{
	return (nStatus != 0) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_status_is_failure(int nStatus)
    __attribute__((alias("gj_status_is_failure")));
