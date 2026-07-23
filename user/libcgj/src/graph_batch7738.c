/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7738: fchdir ENOTDIR errno number stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fchdir_enotdir_num_7738(void);
 *     - Returns 20 (soft Linux ENOTDIR: fd does not refer to a directory).
 *   uint32_t __gj_fchdir_enotdir_num_7738  (alias)
 *   __libcgj_batch7738_marker = "libcgj-batch7738"
 *
 * Exclusive continuum CREATE-ONLY (7731-7740: fchdir fd ok stubs —
 * fd_ok, fd_errorish, fd_pos_ok, at_fdcwd_u, nr_x86_64, arg_count,
 * ebadf_num, enotdir_num, success_rc, continuum + batch_id_7740).
 * Unique gj_fchdir_enotdir_num_7738 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7738_marker[] = "libcgj-batch7738";

/* Soft Linux ENOTDIR. Catalog only; does not set errno. */
#define B7738_ENOTDIR  20u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7738_enotdir_num(void)
{
	return B7738_ENOTDIR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fchdir_enotdir_num_7738 - soft ENOTDIR catalog number for fchdir.
 *
 * Always returns 20. Product-side errno-name tag when fd is open but not
 * a directory; does not call fchdir or touch errno. No parent wires.
 */
uint32_t
gj_fchdir_enotdir_num_7738(void)
{
	(void)NULL;
	return b7738_enotdir_num();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fchdir_enotdir_num_7738(void)
    __attribute__((alias("gj_fchdir_enotdir_num_7738")));
