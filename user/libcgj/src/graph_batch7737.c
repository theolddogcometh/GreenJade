/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7737: fchdir EBADF errno number stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fchdir_ebadf_num_7737(void);
 *     - Returns 9 (soft Linux EBADF: bad file descriptor for fchdir).
 *   uint32_t __gj_fchdir_ebadf_num_7737  (alias)
 *   __libcgj_batch7737_marker = "libcgj-batch7737"
 *
 * Exclusive continuum CREATE-ONLY (7731-7740: fchdir fd ok stubs —
 * fd_ok, fd_errorish, fd_pos_ok, at_fdcwd_u, nr_x86_64, arg_count,
 * ebadf_num, enotdir_num, success_rc, continuum + batch_id_7740).
 * Unique gj_fchdir_ebadf_num_7737 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7737_marker[] = "libcgj-batch7737";

/* Soft Linux EBADF. Catalog only; does not set errno. */
#define B7737_EBADF  9u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7737_ebadf_num(void)
{
	return B7737_EBADF;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fchdir_ebadf_num_7737 - soft EBADF catalog number for fchdir.
 *
 * Always returns 9. Product-side errno-name tag when fd is not open;
 * does not call fchdir or touch errno. No parent wires.
 */
uint32_t
gj_fchdir_ebadf_num_7737(void)
{
	(void)NULL;
	return b7737_ebadf_num();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fchdir_ebadf_num_7737(void)
    __attribute__((alias("gj_fchdir_ebadf_num_7737")));
