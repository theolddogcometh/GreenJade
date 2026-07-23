/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7739: fchdir success return-code stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fchdir_success_rc_7739(void);
 *     - Returns 0 (soft POSIX success return code for fchdir).
 *   uint32_t __gj_fchdir_success_rc_7739  (alias)
 *   __libcgj_batch7739_marker = "libcgj-batch7739"
 *
 * Exclusive continuum CREATE-ONLY (7731-7740: fchdir fd ok stubs —
 * fd_ok, fd_errorish, fd_pos_ok, at_fdcwd_u, nr_x86_64, arg_count,
 * ebadf_num, enotdir_num, success_rc, continuum + batch_id_7740).
 * Unique gj_fchdir_success_rc_7739 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7739_marker[] = "libcgj-batch7739";

/* Soft POSIX success return for fchdir. */
#define B7739_SUCCESS_RC  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7739_success_rc(void)
{
	return B7739_SUCCESS_RC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fchdir_success_rc_7739 - soft fchdir success return-code tag.
 *
 * Always returns 0. Catalog baseline for a successful fchdir return;
 * does not call fchdir. No parent wires.
 */
uint32_t
gj_fchdir_success_rc_7739(void)
{
	(void)NULL;
	return b7739_success_rc();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fchdir_success_rc_7739(void)
    __attribute__((alias("gj_fchdir_success_rc_7739")));
