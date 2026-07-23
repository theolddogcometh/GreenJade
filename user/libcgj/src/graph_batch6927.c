/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6927: copy_file_range same-fd soft detect.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cfr_same_fd_u_6927(uint32_t fd_in, uint32_t fd_out);
 *     - Return 1 if fd_in == fd_out (soft same-descriptor detect for
 *       overlap-aware product logic); else 0.
 *   uint32_t __gj_cfr_same_fd_u_6927  (alias)
 *   __libcgj_batch6927_marker = "libcgj-batch6927"
 *
 * Exclusive continuum CREATE-ONLY (6921-6930: copy_file_range stubs).
 * Unique gj_cfr_same_fd_u_6927 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6927_marker[] = "libcgj-batch6927";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6927_same_fd(uint32_t u32FdIn, uint32_t u32FdOut)
{
	return (u32FdIn == u32FdOut) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfr_same_fd_u_6927 - soft same-fd detect for copy_file_range ends.
 *
 * fd_in:  soft input file descriptor (unsigned form)
 * fd_out: soft output file descriptor (unsigned form)
 *
 * Returns 1 when both ends name the same fd, else 0. Pure equality
 * test; does not check overlap ranges or call copy_file_range.
 * No parent wires.
 */
uint32_t
gj_cfr_same_fd_u_6927(uint32_t u32FdIn, uint32_t u32FdOut)
{
	(void)NULL;
	return b6927_same_fd(u32FdIn, u32FdOut);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cfr_same_fd_u_6927(uint32_t u32FdIn, uint32_t u32FdOut)
    __attribute__((alias("gj_cfr_same_fd_u_6927")));
