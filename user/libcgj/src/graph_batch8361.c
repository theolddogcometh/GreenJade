/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8361: file-descriptor validity predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fd_valid_u_8361(int fd);
 *     - Return 1 if fd >= 0 (non-negative descriptor slot), else 0.
 *       Pure local predicate; does not call fcntl/fstat or probe the
 *       kernel. Negative values (including -1 closed/error) fail.
 *   uint32_t __gj_fd_valid_u_8361  (alias)
 *   __libcgj_batch8361_marker = "libcgj-batch8361"
 *
 * Exclusive continuum CREATE-ONLY (8361-8370: fd/flags bit stubs —
 * fd_valid, fd_stdin_id, fd_stdout_id, fd_stderr_id, oflag_rdonly,
 * oflag_wronly, oflag_rdwr, oflag_creat, oflag_trunc, batch_id_8370).
 * Unique gj_fd_valid_u_8361 surface only; no multi-def.
 * Distinct from gj_fd_valid_hint (batch1667). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8361_marker[] = "libcgj-batch8361";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8361_fd_valid(int iFd)
{
	return (iFd >= 0) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fd_valid_u_8361 - local non-negative fd predicate.
 *
 * fd: descriptor number under test.
 * Returns 1 when fd >= 0, else 0.
 * Screens the classic -1 / error encoding only; a non-negative int may
 * still be closed or unopened in the process. No parent wires.
 */
uint32_t
gj_fd_valid_u_8361(int iFd)
{
	(void)NULL;
	return b8361_fd_valid(iFd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fd_valid_u_8361(int iFd)
    __attribute__((alias("gj_fd_valid_u_8361")));
