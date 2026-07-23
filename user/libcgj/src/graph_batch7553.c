/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7553: sigprocmask SIG_SETMASK how id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sigprocmask_how_setmask_id_7553(void);
 *     - Return soft SIG_SETMASK how code (2): replace process mask.
 *   uint32_t __gj_sigprocmask_how_setmask_id_7553  (alias)
 *   __libcgj_batch7553_marker = "libcgj-batch7553"
 *
 * Exclusive continuum CREATE-ONLY (7551-7560: sigprocmask how stubs —
 * block_id, unblock_id, setmask_id, is_block, is_unblock, is_setmask,
 * how_ok, how_pack, how_errorish, continuum + batch_id_7560). Unique
 * gj_sigprocmask_how_setmask_id_7553 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7553_marker[] = "libcgj-batch7553";

/* Soft SIG_SETMASK: assign set as the process signal mask (how = 2). */
#define B7553_SIG_SETMASK  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7553_how_setmask_id(void)
{
	return B7553_SIG_SETMASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sigprocmask_how_setmask_id_7553 - soft SIG_SETMASK how constant.
 *
 * Always returns 2. Catalog id only; does not call sigprocmask.
 * No parent wires.
 */
uint32_t
gj_sigprocmask_how_setmask_id_7553(void)
{
	(void)NULL;
	return b7553_how_setmask_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sigprocmask_how_setmask_id_7553(void)
    __attribute__((alias("gj_sigprocmask_how_setmask_id_7553")));
