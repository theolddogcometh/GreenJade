/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7226: select soft FD_CLR (clear-bit) stub.
 *
 * Surface (unique symbols):
 *   uint64_t gj_fd_clr_bit_u_7226(uint64_t word, uint32_t fd);
 *     - Return word with bit (fd % 64) cleared when fd < FD_SETSIZE;
 *       otherwise return word unchanged. Soft pure-integer AND-not stub.
 *   uint64_t __gj_fd_clr_bit_u_7226  (alias)
 *   __libcgj_batch7226_marker = "libcgj-batch7226"
 *
 * Exclusive continuum CREATE-ONLY (7221-7230: select fd_set bit stubs —
 * fd_setsize, fd_nfdbits, fd_word_index, fd_bit_mask, fd_set_bit,
 * fd_clr_bit, fd_isset, fd_zero_word, fd_in_setsize, batch_id_7230).
 * Unique gj_fd_clr_bit_u_7226 surface only; no multi-def.
 * Self-contained; does not call sibling helpers. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7226_marker[] = "libcgj-batch7226";

#define B7226_FD_SETSIZE ((uint32_t)1024u)
#define B7226_NFDBITS    ((uint32_t)64u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b7226_fd_clr_bit(uint64_t uWord, uint32_t uFd)
{
	uint32_t uBit;

	if (uFd >= B7226_FD_SETSIZE) {
		return uWord;
	}
	uBit = uFd % B7226_NFDBITS;
	return uWord & ~((uint64_t)1u << uBit);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fd_clr_bit_u_7226 - soft FD_CLR-style bit clear (AND-not one word).
 *
 * word: current 64-bit soft fds_bits[] element.
 * fd:   descriptor number; valid range [0, FD_SETSIZE).
 * Returns word with bit (fd % 64) cleared, or word if fd invalid.
 * No parent wires.
 */
uint64_t
gj_fd_clr_bit_u_7226(uint64_t uWord, uint32_t uFd)
{
	(void)NULL;
	return b7226_fd_clr_bit(uWord, uFd);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_fd_clr_bit_u_7226(uint64_t uWord, uint32_t uFd)
    __attribute__((alias("gj_fd_clr_bit_u_7226")));
