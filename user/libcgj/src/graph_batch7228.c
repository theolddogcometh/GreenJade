/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7228: select soft FD_ZERO (zero one word) stub.
 *
 * Surface (unique symbols):
 *   uint64_t gj_fd_zero_word_7228(void);
 *     - Return 0 — soft empty fds_bits[] element (FD_ZERO one word).
 *   uint64_t __gj_fd_zero_word_7228  (alias)
 *   __libcgj_batch7228_marker = "libcgj-batch7228"
 *
 * Exclusive continuum CREATE-ONLY (7221-7230: select fd_set bit stubs —
 * fd_setsize, fd_nfdbits, fd_word_index, fd_bit_mask, fd_set_bit,
 * fd_clr_bit, fd_isset, fd_zero_word, fd_in_setsize, batch_id_7230).
 * Unique gj_fd_zero_word_7228 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7228_marker[] = "libcgj-batch7228";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b7228_fd_zero_word(void)
{
	return 0ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fd_zero_word_7228 - soft FD_ZERO for one fds_bits[] word.
 *
 * Always returns 0. Catalog helper only; does not zero a full fd_set
 * buffer. No parent wires.
 */
uint64_t
gj_fd_zero_word_7228(void)
{
	(void)NULL;
	return b7228_fd_zero_word();
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_fd_zero_word_7228(void)
    __attribute__((alias("gj_fd_zero_word_7228")));
