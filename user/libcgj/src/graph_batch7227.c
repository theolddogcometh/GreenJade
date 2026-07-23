/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7227: select soft FD_ISSET (test-bit) stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fd_isset_u_7227(uint64_t word, uint32_t fd);
 *     - Return 1 when fd < FD_SETSIZE and bit (fd % 64) is set in word,
 *       else 0. Soft pure-integer membership probe.
 *   uint32_t __gj_fd_isset_u_7227  (alias)
 *   __libcgj_batch7227_marker = "libcgj-batch7227"
 *
 * Exclusive continuum CREATE-ONLY (7221-7230: select fd_set bit stubs —
 * fd_setsize, fd_nfdbits, fd_word_index, fd_bit_mask, fd_set_bit,
 * fd_clr_bit, fd_isset, fd_zero_word, fd_in_setsize, batch_id_7230).
 * Unique gj_fd_isset_u_7227 surface only; no multi-def.
 * Self-contained; does not call sibling helpers. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7227_marker[] = "libcgj-batch7227";

#define B7227_FD_SETSIZE ((uint32_t)1024u)
#define B7227_NFDBITS    ((uint32_t)64u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7227_fd_isset(uint64_t uWord, uint32_t uFd)
{
	uint32_t uBit;

	if (uFd >= B7227_FD_SETSIZE) {
		return 0u;
	}
	uBit = uFd % B7227_NFDBITS;
	return ((uWord >> uBit) & 1u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fd_isset_u_7227 - soft FD_ISSET-style bit membership test.
 *
 * word: current 64-bit soft fds_bits[] element.
 * fd:   descriptor number; valid range [0, FD_SETSIZE).
 * Returns 1 if the bit is set and fd is in range, else 0.
 * No parent wires.
 */
uint32_t
gj_fd_isset_u_7227(uint64_t uWord, uint32_t uFd)
{
	(void)NULL;
	return b7227_fd_isset(uWord, uFd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fd_isset_u_7227(uint64_t uWord, uint32_t uFd)
    __attribute__((alias("gj_fd_isset_u_7227")));
