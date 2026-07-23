/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7221: select FD_SETSIZE constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fd_setsize_7221(void);
 *     - Return soft FD_SETSIZE (1024).
 *   uint32_t __gj_fd_setsize_7221  (alias)
 *   __libcgj_batch7221_marker = "libcgj-batch7221"
 *
 * Exclusive continuum CREATE-ONLY (7221-7230: select fd_set bit stubs —
 * fd_setsize, fd_nfdbits, fd_word_index, fd_bit_mask, fd_set_bit,
 * fd_clr_bit, fd_isset, fd_zero_word, fd_in_setsize, batch_id_7230).
 * Unique gj_fd_setsize_7221 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7221_marker[] = "libcgj-batch7221";

/* Soft FD_SETSIZE: maximum fd for classic select fd_set (Linux default). */
#define B7221_FD_SETSIZE ((uint32_t)1024u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7221_fd_setsize(void)
{
	return B7221_FD_SETSIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fd_setsize_7221 - soft FD_SETSIZE constant.
 *
 * Always returns 1024. Catalog constant only; does not call select.
 * No parent wires.
 */
uint32_t
gj_fd_setsize_7221(void)
{
	(void)NULL;
	return b7221_fd_setsize();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fd_setsize_7221(void)
    __attribute__((alias("gj_fd_setsize_7221")));
