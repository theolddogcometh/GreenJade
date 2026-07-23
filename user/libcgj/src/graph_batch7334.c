/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7334: flock LOCK_UN op id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flock_lock_un_id_7334(void);
 *     - Return soft LOCK_UN constant (8).
 *   uint32_t __gj_flock_lock_un_id_7334  (alias)
 *   __libcgj_batch7334_marker = "libcgj-batch7334"
 *
 * Exclusive continuum CREATE-ONLY (7331-7340: flock op stubs —
 * lock_sh_id, lock_ex_id, lock_nb_id, lock_un_id, has_sh, has_ex,
 * has_nb, ops_ok, ops_pack, batch_id_7340).
 * Unique gj_flock_lock_un_id_7334 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7334_marker[] = "libcgj-batch7334";

/* LOCK_UN: unlock (8). */
#define B7334_LOCK_UN ((uint32_t)8u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7334_flock_lock_un_id(void)
{
	return B7334_LOCK_UN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flock_lock_un_id_7334 - soft LOCK_UN operation constant for flock.
 *
 * Always returns 8 (Linux LOCK_UN). Catalog id only; does not call
 * flock. No parent wires.
 */
uint32_t
gj_flock_lock_un_id_7334(void)
{
	(void)NULL;
	return b7334_flock_lock_un_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flock_lock_un_id_7334(void)
    __attribute__((alias("gj_flock_lock_un_id_7334")));
