/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7332: flock LOCK_EX op id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flock_lock_ex_id_7332(void);
 *     - Return soft LOCK_EX constant (2).
 *   uint32_t __gj_flock_lock_ex_id_7332  (alias)
 *   __libcgj_batch7332_marker = "libcgj-batch7332"
 *
 * Exclusive continuum CREATE-ONLY (7331-7340: flock op stubs —
 * lock_sh_id, lock_ex_id, lock_nb_id, lock_un_id, has_sh, has_ex,
 * has_nb, ops_ok, ops_pack, batch_id_7340).
 * Unique gj_flock_lock_ex_id_7332 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7332_marker[] = "libcgj-batch7332";

/* LOCK_EX: exclusive lock (2). */
#define B7332_LOCK_EX ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7332_flock_lock_ex_id(void)
{
	return B7332_LOCK_EX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flock_lock_ex_id_7332 - soft LOCK_EX operation constant for flock.
 *
 * Always returns 2 (Linux LOCK_EX). Catalog id only; does not call
 * flock. No parent wires.
 */
uint32_t
gj_flock_lock_ex_id_7332(void)
{
	(void)NULL;
	return b7332_flock_lock_ex_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flock_lock_ex_id_7332(void)
    __attribute__((alias("gj_flock_lock_ex_id_7332")));
