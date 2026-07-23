/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7333: flock LOCK_NB op id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flock_lock_nb_id_7333(void);
 *     - Return soft LOCK_NB constant (4).
 *   uint32_t __gj_flock_lock_nb_id_7333  (alias)
 *   __libcgj_batch7333_marker = "libcgj-batch7333"
 *
 * Exclusive continuum CREATE-ONLY (7331-7340: flock op stubs —
 * lock_sh_id, lock_ex_id, lock_nb_id, lock_un_id, has_sh, has_ex,
 * has_nb, ops_ok, ops_pack, batch_id_7340).
 * Unique gj_flock_lock_nb_id_7333 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7333_marker[] = "libcgj-batch7333";

/* LOCK_NB: non-blocking modifier (4). */
#define B7333_LOCK_NB ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7333_flock_lock_nb_id(void)
{
	return B7333_LOCK_NB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flock_lock_nb_id_7333 - soft LOCK_NB operation constant for flock.
 *
 * Always returns 4 (Linux LOCK_NB). Catalog id only; does not call
 * flock. No parent wires.
 */
uint32_t
gj_flock_lock_nb_id_7333(void)
{
	(void)NULL;
	return b7333_flock_lock_nb_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flock_lock_nb_id_7333(void)
    __attribute__((alias("gj_flock_lock_nb_id_7333")));
