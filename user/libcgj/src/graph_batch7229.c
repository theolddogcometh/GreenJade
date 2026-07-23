/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7229: select fd-in-FD_SETSIZE predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fd_in_setsize_u_7229(uint32_t fd);
 *     - Return 1 if fd < soft FD_SETSIZE (1024), else 0.
 *   uint32_t __gj_fd_in_setsize_u_7229  (alias)
 *   __libcgj_batch7229_marker = "libcgj-batch7229"
 *
 * Exclusive continuum CREATE-ONLY (7221-7230: select fd_set bit stubs —
 * fd_setsize, fd_nfdbits, fd_word_index, fd_bit_mask, fd_set_bit,
 * fd_clr_bit, fd_isset, fd_zero_word, fd_in_setsize, batch_id_7230).
 * Unique gj_fd_in_setsize_u_7229 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7229_marker[] = "libcgj-batch7229";

#define B7229_FD_SETSIZE ((uint32_t)1024u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7229_fd_in_setsize(uint32_t uFd)
{
	return (uFd < B7229_FD_SETSIZE) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fd_in_setsize_u_7229 - 1 if fd is inside soft FD_SETSIZE.
 *
 * fd: descriptor number under test.
 * Returns 1 when fd < 1024, else 0.
 * Pure integer compare; does not call select. No parent wires.
 */
uint32_t
gj_fd_in_setsize_u_7229(uint32_t uFd)
{
	(void)NULL;
	return b7229_fd_in_setsize(uFd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fd_in_setsize_u_7229(uint32_t uFd)
    __attribute__((alias("gj_fd_in_setsize_u_7229")));
