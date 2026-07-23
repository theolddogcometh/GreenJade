/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7224: select fd_set in-word bit mask stub.
 *
 * Surface (unique symbols):
 *   uint64_t gj_fd_bit_mask_u_7224(uint32_t fd);
 *     - Return 1ull << (fd % 64) when fd < FD_SETSIZE; else 0.
 *   uint64_t __gj_fd_bit_mask_u_7224  (alias)
 *   __libcgj_batch7224_marker = "libcgj-batch7224"
 *
 * Exclusive continuum CREATE-ONLY (7221-7230: select fd_set bit stubs —
 * fd_setsize, fd_nfdbits, fd_word_index, fd_bit_mask, fd_set_bit,
 * fd_clr_bit, fd_isset, fd_zero_word, fd_in_setsize, batch_id_7230).
 * Unique gj_fd_bit_mask_u_7224 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7224_marker[] = "libcgj-batch7224";

#define B7224_FD_SETSIZE ((uint32_t)1024u)
#define B7224_NFDBITS    ((uint32_t)64u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b7224_fd_bit_mask(uint32_t uFd)
{
	uint32_t uBit;

	if (uFd >= B7224_FD_SETSIZE) {
		return 0ull;
	}
	uBit = uFd % B7224_NFDBITS;
	return (uint64_t)1u << uBit;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fd_bit_mask_u_7224 - soft in-word bit mask for fd.
 *
 * fd: descriptor number (0 .. FD_SETSIZE-1 valid).
 * Returns (1ull << (fd % 64)) when in range, else 0.
 * Pure integer; does not touch an fd_set. No parent wires.
 */
uint64_t
gj_fd_bit_mask_u_7224(uint32_t uFd)
{
	(void)NULL;
	return b7224_fd_bit_mask(uFd);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_fd_bit_mask_u_7224(uint32_t uFd)
    __attribute__((alias("gj_fd_bit_mask_u_7224")));
