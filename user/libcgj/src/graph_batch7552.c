/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7552: sigprocmask SIG_UNBLOCK how id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sigprocmask_how_unblock_id_7552(void);
 *     - Return soft SIG_UNBLOCK how code (1): unblock signals in set.
 *   uint32_t __gj_sigprocmask_how_unblock_id_7552  (alias)
 *   __libcgj_batch7552_marker = "libcgj-batch7552"
 *
 * Exclusive continuum CREATE-ONLY (7551-7560: sigprocmask how stubs —
 * block_id, unblock_id, setmask_id, is_block, is_unblock, is_setmask,
 * how_ok, how_pack, how_errorish, continuum + batch_id_7560). Unique
 * gj_sigprocmask_how_unblock_id_7552 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7552_marker[] = "libcgj-batch7552";

/* Soft SIG_UNBLOCK: remove set from the process signal mask (how = 1). */
#define B7552_SIG_UNBLOCK  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7552_how_unblock_id(void)
{
	return B7552_SIG_UNBLOCK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sigprocmask_how_unblock_id_7552 - soft SIG_UNBLOCK how constant.
 *
 * Always returns 1. Catalog id only; does not call sigprocmask.
 * No parent wires.
 */
uint32_t
gj_sigprocmask_how_unblock_id_7552(void)
{
	(void)NULL;
	return b7552_how_unblock_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sigprocmask_how_unblock_id_7552(void)
    __attribute__((alias("gj_sigprocmask_how_unblock_id_7552")));
