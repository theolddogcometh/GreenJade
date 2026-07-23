/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7222: select NFDBITS / fd_mask width stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fd_nfdbits_7222(void);
 *     - Return soft NFDBITS (64) — bits per fd_mask word.
 *   uint32_t __gj_fd_nfdbits_7222  (alias)
 *   __libcgj_batch7222_marker = "libcgj-batch7222"
 *
 * Exclusive continuum CREATE-ONLY (7221-7230: select fd_set bit stubs —
 * fd_setsize, fd_nfdbits, fd_word_index, fd_bit_mask, fd_set_bit,
 * fd_clr_bit, fd_isset, fd_zero_word, fd_in_setsize, batch_id_7230).
 * Unique gj_fd_nfdbits_7222 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7222_marker[] = "libcgj-batch7222";

/* Soft NFDBITS: bits in one fd_mask / fds_bits[] element (64-bit long). */
#define B7222_NFDBITS ((uint32_t)64u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7222_fd_nfdbits(void)
{
	return B7222_NFDBITS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fd_nfdbits_7222 - soft NFDBITS constant.
 *
 * Always returns 64. Catalog constant only; does not call select.
 * No parent wires.
 */
uint32_t
gj_fd_nfdbits_7222(void)
{
	(void)NULL;
	return b7222_fd_nfdbits();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fd_nfdbits_7222(void)
    __attribute__((alias("gj_fd_nfdbits_7222")));
