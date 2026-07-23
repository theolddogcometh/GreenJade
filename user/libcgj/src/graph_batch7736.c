/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7736: fchdir argument count stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fchdir_arg_count_7736(void);
 *     - Returns 1 (soft: fchdir takes a single fd argument).
 *   uint32_t __gj_fchdir_arg_count_7736  (alias)
 *   __libcgj_batch7736_marker = "libcgj-batch7736"
 *
 * Exclusive continuum CREATE-ONLY (7731-7740: fchdir fd ok stubs —
 * fd_ok, fd_errorish, fd_pos_ok, at_fdcwd_u, nr_x86_64, arg_count,
 * ebadf_num, enotdir_num, success_rc, continuum + batch_id_7740).
 * Unique gj_fchdir_arg_count_7736 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7736_marker[] = "libcgj-batch7736";

/* Soft: fchdir(int fd) — one argument. */
#define B7736_ARG_COUNT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7736_arg_count(void)
{
	return B7736_ARG_COUNT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fchdir_arg_count_7736 - soft fchdir argument arity tag.
 *
 * Always returns 1. Pure-data layout tag for the single fd argument.
 * Does not call fchdir. No parent wires.
 */
uint32_t
gj_fchdir_arg_count_7736(void)
{
	(void)NULL;
	return b7736_arg_count();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fchdir_arg_count_7736(void)
    __attribute__((alias("gj_fchdir_arg_count_7736")));
