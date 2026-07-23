/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7551: sigprocmask SIG_BLOCK how id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sigprocmask_how_block_id_7551(void);
 *     - Return soft SIG_BLOCK how code (0): block signals in set.
 *   uint32_t __gj_sigprocmask_how_block_id_7551  (alias)
 *   __libcgj_batch7551_marker = "libcgj-batch7551"
 *
 * Exclusive continuum CREATE-ONLY (7551-7560: sigprocmask how stubs —
 * block_id, unblock_id, setmask_id, is_block, is_unblock, is_setmask,
 * how_ok, how_pack, how_errorish, continuum + batch_id_7560). Unique
 * gj_sigprocmask_how_block_id_7551 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7551_marker[] = "libcgj-batch7551";

/* Soft SIG_BLOCK: add set to the process signal mask (how = 0). */
#define B7551_SIG_BLOCK  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7551_how_block_id(void)
{
	return B7551_SIG_BLOCK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sigprocmask_how_block_id_7551 - soft SIG_BLOCK how constant.
 *
 * Always returns 0. Catalog id only; does not call sigprocmask.
 * No parent wires.
 */
uint32_t
gj_sigprocmask_how_block_id_7551(void)
{
	(void)NULL;
	return b7551_how_block_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sigprocmask_how_block_id_7551(void)
    __attribute__((alias("gj_sigprocmask_how_block_id_7551")));
