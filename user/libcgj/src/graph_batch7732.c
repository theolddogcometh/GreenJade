/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7732: fchdir fd errorish/negative stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fchdir_fd_errorish_7732(int32_t fd);
 *     - Return 1 if fd < 0 (soft negative / invalid fd); else 0.
 *   uint32_t __gj_fchdir_fd_errorish_7732  (alias)
 *   __libcgj_batch7732_marker = "libcgj-batch7732"
 *
 * Exclusive continuum CREATE-ONLY (7731-7740: fchdir fd ok stubs —
 * fd_ok, fd_errorish, fd_pos_ok, at_fdcwd_u, nr_x86_64, arg_count,
 * ebadf_num, enotdir_num, success_rc, continuum + batch_id_7740).
 * Unique gj_fchdir_fd_errorish_7732 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7732_marker[] = "libcgj-batch7732";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7732_fd_errorish(int32_t nFd)
{
	return (nFd < 0) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fchdir_fd_errorish_7732 - soft inverse of non-negative fd check.
 *
 * fd: soft file-descriptor integer (directory candidate)
 *
 * Returns 1 when fd is negative (errorish for fchdir), else 0.
 * Soft pure-data gate; does not call fchdir. No parent wires.
 */
uint32_t
gj_fchdir_fd_errorish_7732(int32_t nFd)
{
	(void)NULL;
	return b7732_fd_errorish(nFd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fchdir_fd_errorish_7732(int32_t nFd)
    __attribute__((alias("gj_fchdir_fd_errorish_7732")));
