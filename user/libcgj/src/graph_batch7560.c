/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7560: sigprocmask how continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sigprocmask_how_continuum_ok_7560(void);
 *     - Returns 1 (soft lamp: 7551-7560 sigprocmask how stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_7560(void);
 *     - Returns the compile-time graph batch number for this TU (7560).
 *   uint32_t __gj_sigprocmask_how_continuum_ok_7560  (alias)
 *   uint32_t __gj_batch_id_7560  (alias)
 *   __libcgj_batch7560_marker = "libcgj-batch7560"
 *
 * Exclusive continuum CREATE-ONLY (7551-7560: sigprocmask how stubs —
 * block_id, unblock_id, setmask_id, is_block, is_unblock, is_setmask,
 * how_ok, how_pack, how_errorish, continuum + batch_id_7560). Unique
 * surfaces only; no multi-def. Does NOT redefine bare gj_batch_id. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7560_marker[] = "libcgj-batch7560";

#define B7560_CONTINUUM_OK  1u
#define B7560_BATCH_ID      7560u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7560_continuum(void)
{
	return B7560_CONTINUUM_OK;
}

static uint32_t
b7560_id(void)
{
	return B7560_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sigprocmask_how_continuum_ok_7560 - continuum-ready tag for 7551-7560.
 *
 * Always returns 1. Soft pure-data product tag that the sigprocmask how
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_sigprocmask_how_continuum_ok_7560(void)
{
	(void)NULL;
	return b7560_continuum();
}

/*
 * gj_batch_id_7560 - report this TU's graph batch number.
 *
 * Always returns 7560.
 */
uint32_t
gj_batch_id_7560(void)
{
	return b7560_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_sigprocmask_how_continuum_ok_7560(void)
    __attribute__((alias("gj_sigprocmask_how_continuum_ok_7560")));

uint32_t __gj_batch_id_7560(void)
    __attribute__((alias("gj_batch_id_7560")));
