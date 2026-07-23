/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7223: select fd_set word-index stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fd_word_index_u_7223(uint32_t fd);
 *     - Return fds_bits[] word index (fd / 64) when fd < FD_SETSIZE;
 *       else return 0xFFFFFFFFu.
 *   uint32_t __gj_fd_word_index_u_7223  (alias)
 *   __libcgj_batch7223_marker = "libcgj-batch7223"
 *
 * Exclusive continuum CREATE-ONLY (7221-7230: select fd_set bit stubs —
 * fd_setsize, fd_nfdbits, fd_word_index, fd_bit_mask, fd_set_bit,
 * fd_clr_bit, fd_isset, fd_zero_word, fd_in_setsize, batch_id_7230).
 * Unique gj_fd_word_index_u_7223 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7223_marker[] = "libcgj-batch7223";

#define B7223_FD_SETSIZE ((uint32_t)1024u)
#define B7223_NFDBITS    ((uint32_t)64u)
#define B7223_INVALID    ((uint32_t)0xFFFFFFFFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7223_fd_word_index(uint32_t uFd)
{
	if (uFd >= B7223_FD_SETSIZE) {
		return B7223_INVALID;
	}
	return uFd / B7223_NFDBITS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fd_word_index_u_7223 - soft fd_set word index for fd.
 *
 * fd: descriptor number (0 .. FD_SETSIZE-1 valid).
 * Returns fd / 64 when in range, else 0xFFFFFFFFu.
 * Pure integer; does not touch an fd_set. No parent wires.
 */
uint32_t
gj_fd_word_index_u_7223(uint32_t uFd)
{
	(void)NULL;
	return b7223_fd_word_index(uFd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fd_word_index_u_7223(uint32_t uFd)
    __attribute__((alias("gj_fd_word_index_u_7223")));
