/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7331: flock LOCK_SH op id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flock_lock_sh_id_7331(void);
 *     - Return soft LOCK_SH constant (1).
 *   uint32_t __gj_flock_lock_sh_id_7331  (alias)
 *   __libcgj_batch7331_marker = "libcgj-batch7331"
 *
 * Exclusive continuum CREATE-ONLY (7331-7340: flock op stubs —
 * lock_sh_id, lock_ex_id, lock_nb_id, lock_un_id, has_sh, has_ex,
 * has_nb, ops_ok, ops_pack, batch_id_7340).
 * Unique gj_flock_lock_sh_id_7331 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7331_marker[] = "libcgj-batch7331";

/* LOCK_SH: shared lock (1). */
#define B7331_LOCK_SH ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7331_flock_lock_sh_id(void)
{
	return B7331_LOCK_SH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flock_lock_sh_id_7331 - soft LOCK_SH operation constant for flock.
 *
 * Always returns 1 (Linux LOCK_SH). Catalog id only; does not call
 * flock. No parent wires.
 */
uint32_t
gj_flock_lock_sh_id_7331(void)
{
	(void)NULL;
	return b7331_flock_lock_sh_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flock_lock_sh_id_7331(void)
    __attribute__((alias("gj_flock_lock_sh_id_7331")));
