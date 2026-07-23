/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7225: select soft FD_SET (set-bit) stub.
 *
 * Surface (unique symbols):
 *   uint64_t gj_fd_set_bit_u_7225(uint64_t word, uint32_t fd);
 *     - Return word with bit (fd % 64) set when fd < FD_SETSIZE;
 *       otherwise return word unchanged. Soft pure-integer OR stub.
 *   uint64_t __gj_fd_set_bit_u_7225  (alias)
 *   __libcgj_batch7225_marker = "libcgj-batch7225"
 *
 * Exclusive continuum CREATE-ONLY (7221-7230: select fd_set bit stubs —
 * fd_setsize, fd_nfdbits, fd_word_index, fd_bit_mask, fd_set_bit,
 * fd_clr_bit, fd_isset, fd_zero_word, fd_in_setsize, batch_id_7230).
 * Unique gj_fd_set_bit_u_7225 surface only; no multi-def.
 * Self-contained; does not call sibling helpers. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7225_marker[] = "libcgj-batch7225";

#define B7225_FD_SETSIZE ((uint32_t)1024u)
#define B7225_NFDBITS    ((uint32_t)64u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b7225_fd_set_bit(uint64_t uWord, uint32_t uFd)
{
	uint32_t uBit;

	if (uFd >= B7225_FD_SETSIZE) {
		return uWord;
	}
	uBit = uFd % B7225_NFDBITS;
	return uWord | ((uint64_t)1u << uBit);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fd_set_bit_u_7225 - soft FD_SET-style bit set (OR into one word).
 *
 * word: current 64-bit soft fds_bits[] element.
 * fd:   descriptor number; valid range [0, FD_SETSIZE).
 * Returns word with bit (fd % 64) set, or word if fd invalid.
 * No parent wires.
 */
uint64_t
gj_fd_set_bit_u_7225(uint64_t uWord, uint32_t uFd)
{
	(void)NULL;
	return b7225_fd_set_bit(uWord, uFd);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_fd_set_bit_u_7225(uint64_t uWord, uint32_t uFd)
    __attribute__((alias("gj_fd_set_bit_u_7225")));
